# libertuscode
Single header utility libraries with APL2, Boost, BSD, MIT, or compatible licenses. This code is based on work of various open source project constributions by some very generous people. All credits and original licensing are at the top of the files.

## lc_image
JPG and PNG loader. JPG baseline only, progressive JPGs will not work.
```c++
  // Loading from file
  int w, h, c;
  unsigned char* data = lc_load_image("test_001.png", &w, &h, &c, 0);
  // do some st uff
  lc_free_image(data);
  
  // Loading from memory  
  int w, h, c;
  unsigned char* data = lc_load_image_mem(size, data, &w, &h, &c, 0);
  // do some st uff
  lc_free_image(data);
```

The last parameter for both ```lc_load_image``` and ```lc_load_image_mem``` can be either 0, 1, 2, 3, 4.

| required_channel  | # channels loaded |
| ------------- | ------------- |
| 0  | JPG=3, PNG=4  |
| 1  | 1  |
| 2  | 2  |
| 3  | 3  |
| 4  | 4, A=0xFF  |


## lc_image_resize
Image resize with various filters:

| filter name/constant |
| --- |
| LC_FILTER_BOX |
| LC_FILTER_TRIANGLE |
| LC_FILTER_QUADRATIC |
| LC_FILTER_CUBIC |
| LC_FILTER_CATMUL_ROM |
| LC_FILTER_MITCHELL |
| LC_FILTER_SINC_BLACKMAN |
| LC_FILTER_GAUSSIAN |
| LC_FILTER_BESSEL_BLACKMAN |

```c++
// 8-bit
void lc_image_resize_uint8(int src_width, int src_height, int src_row_stride, const unsigned char* p_src_data,
                           int dst_wdith, int dst_height, int dst_row_stride, unsigned char* p_dst_data,
                           unsigned int channel_count, lc_filter filter, const lc_filter_args* p_filter_args);

// float
void lc_image_resize_float(int src_width, int src_height, int src_row_stride, const float* p_src_data,
                           int dst_wdith, int dst_height, int dst_row_stride, float* p_dst_data,
                           unsigned int channel_count, lc_filter filter, const lc_filter_args* p_filter_args);

```
The last paraemter for both ```lc_image_resize_uint8``` and ```lc_image_resize_float``` can be ```NULL``` and defaults will be used.
