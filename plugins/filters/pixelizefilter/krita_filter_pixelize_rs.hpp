#ifndef KRITA_FILTER_PIXELIZE_RS_HPP
#define KRITA_FILTER_PIXELIZE_RS_HPP

extern "C" {

void krita_filter_pixelize_rs_process_block(KisSequentialConstIterator *src_it,
                                            KisSequentialIterator *dst_it,
                                            qint32 pixel_size,
                                            qint32 pixelize_width,
                                            qint32 pixelize_height,
                                            const KoMixColorsOp *ko_mix_colors_op,
                                            quint8 *working_buffer,
                                            quint32 num_colors,
                                            quint8 *pixel_color_data);

} // extern "C"

#endif
