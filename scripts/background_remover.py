from PIL import Image
import sys

def remove_background(input_path, output_path, tolerance=30):
    img = Image.open(input_path).convert("RGBA")
    pixels = img.load()
    width, height = img.size

    # Sample the background color from a corner pixel (assumes background touches the edge)
    bg_color = pixels[0, 0][:3]  # (R, G, B) of top-left pixel

    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            # Distance from background color, per channel
            if abs(r - bg_color[0]) <= tolerance and \
               abs(g - bg_color[1]) <= tolerance and \
               abs(b - bg_color[2]) <= tolerance:
                pixels[x, y] = (r, g, b, 0)  # make transparent
    img.save(output_path, "PNG")
    print(f"Saved: {output_path} (background color detected: {bg_color})")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python remove_bg.py <input.png> <output.png> [tolerance]")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]
    tolerance = int(sys.argv[3]) if len(sys.argv) > 3 else 30

    remove_background(input_path, output_path, tolerance)