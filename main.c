#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./deps/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./deps/stb/stb_image_write.h"

// Usage: ./steg e file image
//        ./steg d image
int main(int argc, char **argv) {

  int encode = 0;

  if (argv[1][0] == 'e') {
    encode = 1;
  }

  FILE *file;
  long file_size = 0;
  int width, height, channels;
  unsigned char *img;
  if (encode) {
    // Load file
    file = fopen(argv[2], "rb");
    if (!file) {
      perror("Could not read file");
      return 1;
    }
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    // Load image
    img = stbi_load(argv[3], &width, &height, &channels, 0);
  } else {
    file = fopen("out.txt", "wb");
    if (!file) {
      perror("Could not create file");
      return 1;
    }
    // Load image
    img = stbi_load(argv[2], &width, &height, &channels, 0);
  }

  if (img == NULL) {
    perror("Could not read img");
    fclose(file);
    return 1;
  }

  if (encode) {
    if (((width * height * 3) / 4) < file_size) {
      perror("Image to small for file");
      fclose(file);
      return 1;
    }

    // Load file into mem
    unsigned char *buffer = (unsigned char *)malloc(file_size);
    size_t bytes_read = fread(buffer, 1, file_size, file);

    for (unsigned char *fb = buffer, *ib = img; fb < buffer + file_size;
         fb++, ib += 4) {
      *(ib + 0) = (*(ib + 0) & 0xFC) | ((*fb & 0x03) >> 0);
      *(ib + 1) = (*(ib + 1) & 0xFC) | ((*fb & 0x0C) >> 2);
      *(ib + 2) = (*(ib + 2) & 0xFC) | ((*fb & 0x30) >> 4);
      *(ib + 3) = (*(ib + 3) & 0xFC) | ((*fb & 0xC0) >> 6);
    }

    // for (unsigned char *fb = buffer, *ib = img; fb < buffer + file_size;
    //      fb++, ib += 2) {
    //   *(ib + 0) = (*(ib + 0) & 0xF0) | ((*fb & 0x0F) >> 0);
    //   *(ib + 1) = (*(ib + 1) & 0xF0) | ((*fb & 0xF0) >> 4);
    // }

    stbi_write_png("out.png", width, height, channels, img, width * channels);
    free(buffer);
  } else {
    file_size = (width * height * 3) / 4;
    unsigned char *buffer = (unsigned char *)malloc(file_size);

    for (unsigned char *fb = buffer, *ib = img; fb < buffer + file_size;
         fb++, ib += 4) {
      *fb = ((*(ib + 3) & 0x03) << 6) | ((*(ib + 2) & 0x03) << 4) |
            ((*(ib + 1) & 0x03) << 2) | ((*(ib + 0) & 0x03) << 0);
    }
    fwrite(buffer, 1, file_size, file);
  }

  // Clean up
  fclose(file);

  return 0;
}
