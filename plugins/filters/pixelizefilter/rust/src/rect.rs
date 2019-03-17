#[derive(Clone, Eq, PartialEq, Debug)]
pub(crate) struct Rect {
    left: i32,
    top: i32,
    width: i32,
    height: i32,
}

impl Rect {
    pub fn new(left: i32, top: i32, width: i32, height: i32) -> Self {
        Self {
            left,
            top,
            width,
            height,
        }
    }

    pub fn left(&self) -> i32 {
        self.left
    }

    pub fn top(&self) -> i32 {
        self.top
    }

    pub fn width(&self) -> i32 {
        self.width
    }

    pub fn height(&self) -> i32 {
        self.height
    }

    pub fn intersected(&self, other: &Self) -> Option<Self> {
        use std::cmp::{max, min};
        let left = max(self.left, other.left);
        let top = max(self.top, other.top);
        let right = min(self.left + self.width, other.left + other.width);
        let bottom = min(self.top + self.height, other.top + other.height);
        if left >= right || top >= bottom {
            return None;
        }
        Some(Self {
            left,
            top,
            width: right - left,
            height: bottom - top,
        })
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_intersect_simple() {
        assert_eq!(
            Rect::new(0, 20, 10, 10).intersected(&Rect::new(5, 25, 10, 10)),
            Some(Rect::new(5, 25, 5, 5))
        );
        assert_eq!(
            Rect::new(5, 25, 10, 10).intersected(&Rect::new(0, 20, 10, 10)),
            Some(Rect::new(5, 25, 5, 5))
        );
    }

    #[test]
    fn test_intersect_non_overlapping() {
        assert_eq!(
            Rect::new(0, 20, 10, 10).intersected(&Rect::new(20, 40, 10, 10)),
            None
        );
        assert_eq!(
            Rect::new(0, 20, 10, 10).intersected(&Rect::new(10, 30, 10, 10)),
            None
        );
        assert_eq!(
            Rect::new(0, 20, 10, 10).intersected(&Rect::new(5, 30, 10, 10)),
            None
        );
        assert_eq!(
            Rect::new(0, 20, 10, 10).intersected(&Rect::new(10, 25, 10, 10)),
            None
        );
    }
}
