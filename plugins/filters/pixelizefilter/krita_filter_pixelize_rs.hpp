#ifndef KRITA_FILTER_PIXELIZE_RS_HPP
#define KRITA_FILTER_PIXELIZE_RS_HPP

extern "C" {

void krita_filter_pixelize_rs_process_whole(KisPaintDeviceSP *device,
                                            qint32 device_bounds_left,
                                            qint32 device_bounds_top,
                                            qint32 device_bounds_width,
                                            qint32 device_bounds_height,
                                            qint32 apply_left,
                                            qint32 apply_top,
                                            qint32 apply_width,
                                            qint32 apply_height,
                                            qint32 first_col,
                                            qint32 first_row,
                                            qint32 last_col,
                                            qint32 last_row,
                                            qint32 pixel_size,
                                            qint32 pixelize_width,
                                            qint32 pixelize_height,
                                            const KoMixColorsOp *mix_op,
                                            quint8 *pixel_color_data,
                                            KoUpdater *progress_updater);

void krita_filter_pixelize_rs_process_block(KisPaintDeviceSP *device,
                                            qint32 src_left,
                                            qint32 src_top,
                                            qint32 src_width,
                                            qint32 src_height,
                                            qint32 dst_left,
                                            qint32 dst_top,
                                            qint32 dst_width,
                                            qint32 dst_height,
                                            qint32 pixel_size,
                                            qint32 pixelize_width,
                                            qint32 pixelize_height,
                                            const KoMixColorsOp *ko_mix_colors_op,
                                            quint8 *working_buffer,
                                            quint32 num_colors,
                                            quint8 *pixel_color_data);

} // extern "C"

#endif
