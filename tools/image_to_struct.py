from PIL import Image
import struct
import os

dir = os.getcwd()
print(dir)
img_path = dir + '\\imgs\\Climb_2.JPG'
file_path = dir + '\\img_data\\climb_2.h'
img = Image.open(img_path)
file = open(file_path, 'w')

image_height = img.size[1]
image_width = img.size[0]

header = '#include "pico/stdlib.h"\n\n' + \
         'uint32_t climb_height_2 = ' + str(image_height) + ';\n' + \
         'uint32_t climb_width_2 = ' + str(image_width) + ';\n\n'

body = 'uint8_t __in_flash("image_data") __attribute__((aligned(256))) climb_2[] = {'



pix = img.load()
line = bytes()
line_count = 0
first_byte = True
for h in range(image_height):
    for w in range(image_width):
        if not first_byte:
            body += ','
        else:
            first_byte = False
        R = pix[w, h][0] >> 3
        G = pix[w, h][1] >> 2
        B = pix[w, h][2] >> 3

        rgb = (R << 11) | (G << 5) | B
        line = struct.pack('<H', rgb)
        bytes_string = line.hex(',')
        lower_byte = '0x' + bytes_string.split(",")[0]
        upper_byte = '0x' + bytes_string.split(",")[1]
        body += lower_byte + ',' +upper_byte
        # Send image data by multiple of DISPLAY_WIDTH bytes
body += "};\n\n"

body += "uint32_t climb_2_size = sizeof(climb)/sizeof(uint8_t);\n"

file.write(header)
file.write(body)