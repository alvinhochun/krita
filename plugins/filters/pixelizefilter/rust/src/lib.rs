#![deny(anonymous_parameters)]
#![deny(bare_trait_objects)]
#![deny(elided_lifetimes_in_paths)]
#![warn(clippy::all)]

mod image;
mod krita_ffi;

use image::PixelIterator;
use krita_ffi::KisSequentialConstIterator;
use krita_ffi::KisSequentialIterator;
use krita_ffi::KisPaintDeviceSP;
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
    device: *mut KisPaintDeviceSP,
    src_left: i32,
    src_top: i32,
    src_width: i32,
    src_height: i32,
    dst_left: i32,
    dst_top: i32,
    dst_width: i32,
    dst_height: i32,
    pixel_size: i32,
    pixelize_width: i32,
    pixelize_height: i32,
    mix_op: *const KoMixColorsOp,
    working_buffer: *mut u8,
    num_colors: u32,
    pixel_color_data: *mut KoColorData,
) {
    catch_unwind_abort(|| {
        let mix_op = unsafe { mix_op.as_ref() }.expect("Expected mix_op to not be null");

        if working_buffer.is_null() {
            panic!("Expected working_buffer to not be null");
        }
        let working_buffer = {
            let len = pixelize_width as usize * pixelize_height as usize * pixel_size as usize;
            unsafe { std::slice::from_raw_parts_mut(working_buffer, len) }
        };
        let mut working_buffer_it = working_buffer.chunks_exact_mut(pixel_size as usize);
        let mut src_it = KisSequentialConstIterator::new(device, src_left, src_top, src_width, src_height);
        let mut src_it = unsafe { PixelIterator::new(&mut src_it, pixel_size as u32) };
        while let Some(src_data) = src_it.next_old_raw_data() {
            let working_pixel = working_buffer_it
                .next()
                .expect("Expected working_buffer to have enough space for src_it");
            working_pixel.copy_from_slice(src_data);
        }

        // mix all the colors
        unsafe {
            mix_op.mix_colors(working_buffer.as_ptr(), num_colors, pixel_color_data);
        }

        let pixel_color_data = unsafe {
            std::slice::from_raw_parts(pixel_color_data as *const u8, pixel_size as usize)
        };
        let mut dst_it = KisSequentialIterator::new(device, dst_left, dst_top, dst_width, dst_height);
        let mut dst_it = unsafe { PixelIterator::new(&mut dst_it, pixel_size as u32) };
        while let Some(dst_data) = dst_it.next_raw_data_mut() {
            dst_data.copy_from_slice(pixel_color_data);
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
