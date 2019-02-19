mod krita_ffi;

use krita_ffi::KisSequentialConstIterator;
use krita_ffi::KisSequentialIterator;
use krita_ffi::KoColorData;
use krita_ffi::KoMixColorsOp;

/// Invokes a closure and aborts if an unwinding panic occurs.
///
/// This must be used in any exported functions callable from C/C++ code, as
/// unwinding from Rust code into external caller is undefined behaviour. One
/// must not assume an unwinding panic will not happen in Rust code.
///
/// An exception is when the specific function provides an alternative
/// mechanism to inform the caller of a panic and it can be certain that the
/// program will be able to properly recover from a panic condition, which in
/// such cases it is still an absolute requirement to wrap the code in
/// `std::panic::catch_unwind` and manually handle the error.
fn catch_unwind_abort<F: FnOnce() -> R + std::panic::UnwindSafe, R>(f: F) -> R {
    std::panic::catch_unwind(f).unwrap_or_else(|_| {
        std::process::abort();
    })
}

#[no_mangle]
pub extern "C" fn krita_filter_pixelize_rs_process_block(
    src_it: *mut KisSequentialConstIterator,
    dst_it: *mut KisSequentialIterator,
    pixel_size: i32,
    _pixelize_width: i32,
    _pixelize_height: i32,
    ko_mix_colors_op: *const KoMixColorsOp,
    working_buffer: *mut u8,
    num_colors: u32,
    pixel_color_data: *mut KoColorData,
) {
    catch_unwind_abort(|| {
        let src_it = unsafe { &mut *src_it };
        let dst_it = unsafe { &mut *dst_it };
        let mix_op = unsafe { &*ko_mix_colors_op };

        let mut buffer_ptr = working_buffer;
        while src_it.next_pixel() {
            unsafe {
                std::ptr::copy_nonoverlapping(
                    src_it.old_raw_data_ptr(),
                    buffer_ptr,
                    pixel_size as usize,
                );
                buffer_ptr = buffer_ptr.offset(pixel_size as isize);
            }
        }

        // mix all the colors
        unsafe {
            mix_op.mix_colors(working_buffer, num_colors, pixel_color_data);
        }

        while dst_it.next_pixel() {
            unsafe {
                std::ptr::copy_nonoverlapping(
                    pixel_color_data as *const u8,
                    dst_it.raw_data_ptr(),
                    pixel_size as usize,
                );
            }
        }
    })
}

#[cfg(test)]
mod tests {
    // /// This test cannot be made to pass unless [rust-lang/rust#32512][1] is
    // /// resolved.
    // /// [1]: https://github.com/rust-lang/rust/issues/32512
    // #[test]
    // fn test_should_abort() {
    //     super::catch_unwind_abort(|| panic!("test panic"));
    // }
}
