import os
import cv2
import numpy as np
from pathlib import Path

def create_single_button(item_path, output_path, background_path, item_size=(500, 500),
                        shadow_color=(0, 0, 0, 180), shadow_offset=(5, 5), blur_radius=5,
                        font_scale=1, font_thickness=2, text_padding=10, fore_ground_top_margin=0, fore_ground_left_margin=0):
    """
    Create a single button image by processing an item image with shadow effects and placing it on a background.
    
    Args:
        item_path (str): Path to the item image file
        output_path (str): Path to save the processed button image
        background_path (str): Path to the background image
        item_size (tuple): Target size (width, height) for item (default: (500, 500))
        shadow_color (tuple): RGBA color for shadow (default: black with alpha 180)
        shadow_offset (tuple): (x, y) offset for shadow (default: (5, 5))
        blur_radius (int): Gaussian blur radius for shadow (default: 5)
        font_scale (float): Font scale for text (default: 1)
        font_thickness (int): Font thickness for text (default: 2)
        text_padding (int): Padding between text and bottom of image (default: 10)
        
    Returns:
        bool: True if successful, False otherwise
    """
    try:
        # Load background image
        background = cv2.imread(background_path, cv2.IMREAD_UNCHANGED)
        if background is None:
            raise ValueError(f"Could not load background image at {background_path}")
        
        # If background doesn't have alpha channel, add one
        if background.shape[2] == 3:
            background = cv2.cvtColor(background, cv2.COLOR_BGR2BGRA)
        
        # Load item image
        item = cv2.imread(item_path, cv2.IMREAD_UNCHANGED)
        if item is None:
            raise ValueError(f"Could not load item image at {item_path}")
        
        # If item doesn't have alpha channel, add one
        if item.shape[2] == 3:
            item = cv2.cvtColor(item, cv2.COLOR_BGR2BGRA)
        
        # Scale down item to target size while maintaining aspect ratio
        item = resize_with_aspect_ratio(item, item_size)
        
        # Verify the item will fit on the background (with shadow offset)
        bg_height, bg_width = background.shape[:2]
        item_height, item_width = item.shape[:2]
        
        required_width = item_width + abs(shadow_offset[0]) + 10  # 10px buffer
        required_height = item_height + abs(shadow_offset[1]) + 10
        
        if required_width > bg_width or required_height > bg_height:
            # Calculate maximum possible item size that would fit
            max_item_width = bg_width - abs(shadow_offset[0]) - 10
            max_item_height = bg_height - abs(shadow_offset[1]) - 10
            max_item_size = (max_item_width, max_item_height)
            
            print(f"Item too large for background, resizing to fit. Original target: {item_size}, New target: {max_item_size}")
            item = resize_with_aspect_ratio(item, max_item_size)
            
            # Verify again
            item_height, item_width = item.shape[:2]
            if (item_width + abs(shadow_offset[0]) > bg_width or 
                item_height + abs(shadow_offset[1]) > bg_height):
                raise ValueError(f"Item cannot fit on background even at reduced size. "
                               f"Background: {bg_width}x{bg_height}, "
                               f"Item: {item_width}x{item_height}, "
                               f"Shadow offset: {shadow_offset}")
        
        # Create a copy of the background for this button
        button_img = background.copy()
        
        # Create shadow effect
        shadow = create_shadow(item, shadow_color, shadow_offset, blur_radius)
        
        # Calculate position to center the item on the background
        x_offset = (button_img.shape[1] - item.shape[1]) // 2 - shadow_offset[0]
        x_offset += fore_ground_left_margin
        y_offset = (button_img.shape[0] - item.shape[0]) // 2 - shadow_offset[1]
        y_offset -= fore_ground_top_margin

        # Composite shadow onto background
        button_img = overlay_image(button_img, shadow, x_offset, y_offset)
        
        # Composite item onto background (centered)
        item_x = (button_img.shape[1] - item.shape[1]) // 2
        item_x += fore_ground_left_margin
        item_y = (button_img.shape[0] - item.shape[0]) // 2
        item_y -= fore_ground_top_margin

        button_img = overlay_image(button_img, item, item_x, item_y)
        
        # Add text (item name without extension)
        item_name = os.path.splitext(os.path.basename(item_path))[0]  # removes .png and path
        button_img = add_text(button_img, item_name, font_scale, font_thickness, text_padding)
        
        # Create output directory if it doesn't exist
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        
        # Save the final button image
        cv2.imwrite(output_path, button_img)
        print(f"Successfully created button image: {output_path}")
        return True
    
    except Exception as e:
        print(f"Error processing {item_path}: {str(e)}")
        return False
    

def resize_with_aspect_ratio(image, target_size):
    """
    Resize image to target size while maintaining aspect ratio.
    The image will be scaled to fit within the target dimensions.
    
    Args:
        image (numpy.ndarray): Input image
        target_size (tuple): Target (width, height)
        
    Returns:
        numpy.ndarray: Resized image
    """
    target_width, target_height = target_size
    height, width = image.shape[:2]
    
    # Calculate aspect ratios
    target_ratio = target_width / target_height
    image_ratio = width / height
    
    # Determine scaling factor
    if image_ratio > target_ratio:
        # Image is wider than target - scale to target width
        scale = target_width / width
    else:
        # Image is taller than target - scale to target height
        scale = target_height / height
    
    # Calculate new dimensions
    new_width = int(width * scale)
    new_height = int(height * scale)
    
    # Resize image
    resized = cv2.resize(image, (new_width, new_height), interpolation=cv2.INTER_AREA)
    
    # Create new image with target size and transparent background
    result = np.zeros((target_height, target_width, 4), dtype=np.uint8)
    
    # Calculate position to center the resized image
    x_offset = (target_width - new_width) // 2
    y_offset = (target_height - new_height) // 2
    
    # Place resized image in center
    result[y_offset:y_offset+new_height, x_offset:x_offset+new_width] = resized
    
    return result

def create_shadow(image, shadow_color, offset, blur_radius):
    """Same as previous implementation"""
    # Create a blank image with same size as input
    shadow = np.zeros_like(image)
    
    # Where the original image has transparency > 0, set shadow color
    alpha_channel = image[:, :, 3]
    mask = alpha_channel > 0
    
    # Set shadow color where mask is True
    for c in range(3):  # for BGR channels
        shadow[:, :, c][mask] = shadow_color[c]
    
    # Set alpha channel for shadow
    shadow[:, :, 3][mask] = shadow_color[3]
    
    # Apply Gaussian blur to shadow
    shadow = cv2.GaussianBlur(shadow, (0, 0), blur_radius)
    
    # Create offset shadow
    rows, cols = shadow.shape[:2]
    M = np.float32([[1, 0, offset[0]], [0, 1, offset[1]]])
    shadow = cv2.warpAffine(shadow, M, (cols, rows))
    
    return shadow

def overlay_image(background, foreground, x_offset, y_offset):
    """Same as previous implementation"""
    # Get dimensions
    bg_h, bg_w = background.shape[:2]
    fg_h, fg_w = foreground.shape[:2]
    
    # Ensure foreground fits within background
    if x_offset < 0 or y_offset < 0 or x_offset + fg_w > bg_w or y_offset + fg_h > bg_h:
        raise ValueError("Foreground image does not fit within background at specified position")
    
    # Extract the alpha channel from the foreground
    fg_alpha = foreground[:, :, 3] / 255.0
    bg_alpha = background[y_offset:y_offset+fg_h, x_offset:x_offset+fg_w, 3] / 255.0
    
    # Calculate combined alpha
    combined_alpha = np.clip(fg_alpha + bg_alpha * (1 - fg_alpha), 0, 1)
    
    # Blend each channel
    for c in range(3):
        background[y_offset:y_offset+fg_h, x_offset:x_offset+fg_w, c] = (
            foreground[:, :, c] * fg_alpha +
            background[y_offset:y_offset+fg_h, x_offset:x_offset+fg_w, c] * bg_alpha * (1 - fg_alpha)
        ) / combined_alpha
    
    # Update alpha channel
    background[y_offset:y_offset+fg_h, x_offset:x_offset+fg_w, 3] = combined_alpha * 255
    
    return background

def add_text(image, text, font_scale, thickness, padding):
    """Same as previous implementation"""
    # Convert to BGR if it's BGRA (OpenCV's putText doesn't handle alpha well)
    text = text.split('_')[0]

    if image.shape[2] == 4:
        img_for_text = cv2.cvtColor(image.copy(), cv2.COLOR_BGRA2BGR)
    else:
        img_for_text = image.copy()
    
    # Get text size
    font = cv2.FONT_HERSHEY_SIMPLEX
    text_size = cv2.getTextSize(text, font, font_scale, thickness)[0]
    
    # Calculate text position (centered, above padding)
    text_x = (image.shape[1] - text_size[0]) // 2
    text_y = image.shape[0] - padding
    
    # Add white text with black outline for better visibility
    cv2.putText(img_for_text, text, (text_x, text_y), font, font_scale, 
                (0, 0, 0), thickness + 2, cv2.LINE_AA)
    cv2.putText(img_for_text, text, (text_x, text_y), font, font_scale, 
                (255, 255, 255), thickness, cv2.LINE_AA)
    
    # If original image had alpha channel, merge it back
    if image.shape[2] == 4:
        img_for_text = cv2.cvtColor(img_for_text, cv2.COLOR_BGR2BGRA)
        img_for_text[:, :, 3] = image[:, :, 3]
    
    return img_for_text

if __name__ == "__main__":
    # Configuration
    input_directory = "H:/workspace/Unreal Projects/MasterMaker4.27/MasterMaker/Content/widgets/thumbnails/vehicles"  # Directory containing item_xxx.png files
    output_directory = "H:/workspace/Unreal Projects/MasterMaker4.27/MasterMaker/Content/widgets/thumbnails/generated_thumbnails/vehicles"  # Directory to save processed buttons
    background_image = "H:/workspace/Unreal Projects/MasterMaker4.27/MasterMaker/Content/widgets/thumbnails/item_button_background.png"  # Path to your background image

    item_files = [f for f in os.listdir(input_directory) if f.endswith('.png')]
    
    for item_file in item_files:
        item_name = os.path.splitext(item_file)[0]  # removes .png

        create_single_button(
            item_path= f"{input_directory}/{item_file}",
            output_path= f"{output_directory}/{item_name}.png", #for normal
            # output_path= f"{output_directory}/{item_name}_pressed.png",  #for pressed
            background_path= background_image,
            item_size=(300, 300),
            shadow_color=(0, 0, 0, 180),
            shadow_offset=(5, 5),
            blur_radius=5,
            font_scale=1,
            font_thickness=2,
            text_padding=90,
            fore_ground_top_margin=70,  # 70 for normal
            # fore_ground_top_margin=50, # 50 for pressed
            fore_ground_left_margin=0
        )