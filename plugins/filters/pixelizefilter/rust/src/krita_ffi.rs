extern "C" {
    fn kisSequentialConstIteratorNextPixelCallback(it: *mut KisSequentialConstIterator) -> bool;
    fn kisSequentialConstIteratorOldRawDataCallback(
        it: *const KisSequentialConstIterator,
    ) -> *const u8;
    fn kisSequentialIteratorNextPixelCallback(it: *mut KisSequentialIterator) -> bool;
    fn kisSequentialIteratorRawDataCallback(it: *mut KisSequentialIterator) -> *mut u8;
    fn koMixColorsOpMixColors(
        ko_mix_colors_op: *const KoMixColorsOp,
        colors: *const u8,
        n_colors: u32,
        dst: *mut KoColorData,
    );
}

#[repr(C)]
pub struct KisSequentialConstIterator {
    _private: [u8; 0],
}

impl KisSequentialConstIterator {
    pub fn next_pixel(&mut self) -> bool {
        unsafe { kisSequentialConstIteratorNextPixelCallback(self) }
    }

    pub fn old_raw_data_ptr(&self) -> *const u8 {
        unsafe { kisSequentialConstIteratorOldRawDataCallback(self) }
    }
}

#[repr(C)]
pub struct KisSequentialIterator {
    _private: [u8; 0],
}

impl KisSequentialIterator {
    pub fn next_pixel(&mut self) -> bool {
        unsafe { kisSequentialIteratorNextPixelCallback(self) }
    }

    pub fn raw_data_ptr(&mut self) -> *mut u8 {
        unsafe { kisSequentialIteratorRawDataCallback(self) }
    }
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
