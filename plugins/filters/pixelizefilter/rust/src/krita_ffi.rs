mod kis_sequential_iterators;

pub use kis_sequential_iterators::{
    KisSequentialConstIterator, KisSequentialIterator, KisSequentialIteratorBase,
};

extern "C" {
    fn koMixColorsOpMixColors(
        ko_mix_colors_op: *const KoMixColorsOp,
        colors: *const u8,
        n_colors: u32,
        dst: *mut KoColorData,
    );
}

#[repr(C)]
pub struct KoColorData {
    _private: [u8; 0],
}

#[repr(C)]
pub struct KoMixColorsOp {
    _private: [u8; 0],
}

impl KoMixColorsOp {
    pub unsafe fn mix_colors(&self, colors: *const u8, n_colors: u32, dst: *mut KoColorData) {
        koMixColorsOpMixColors(self, colors, n_colors, dst);
    }
}

#[repr(C)]
pub struct KisPaintDeviceSP {
    _private: [u8; 0],
}
