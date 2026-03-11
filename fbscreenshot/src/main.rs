use argh::FromArgs;
use image::{ImageBuffer, RgbaImage};
use std::fs::File;
use std::io::Read;
use std::process::exit;
use std::str::FromStr;

use nix::ioctl_read_bad;
use std::os::fd::AsRawFd;

#[derive(Debug, Clone, Copy)]
enum Rotation {
    None,
    Rotate90,
    Rotate180,
    Rotate270,
}

impl FromStr for Rotation {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s {
            "0" => Ok(Rotation::None),
            "90" => Ok(Rotation::Rotate90),
            "180" => Ok(Rotation::Rotate180),
            "270" => Ok(Rotation::Rotate270),
            _ => Err(format!("Invalid rotation angle '{}'. Must be 0, 90, 180, or 270", s)),
        }
    }
}

#[derive(FromArgs)]
/// Take a screenshot of the framebuffer
struct Args {
    /// output file path
    #[argh(positional)]
    output: String,

    /// rotation angle in degrees (0, 90, 180, or 270)
    #[argh(option, short = 'r', default = "Rotation::None")]
    rotation: Rotation,
}

#[repr(C)]
struct FbVarScreeninfo {
    xres: u32,
    yres: u32,
    xres_virtual: u32,
    yres_virtual: u32,
    xoffset: u32,
    yoffset: u32,
    bits_per_pixel: u32,
    grayscale: u32,
    red: FbBitfield,
    green: FbBitfield,
    blue: FbBitfield,
    transp: FbBitfield,
}

#[repr(C)]
struct FbBitfield {
    offset: u32,
    length: u32,
    msb_right: u32,
}

ioctl_read_bad!(fbioget_vscreeninfo, 0x4600, FbVarScreeninfo);

fn fbscreenshot(output: &str, rotation: Rotation) -> Result<(), Box<dyn std::error::Error>> {    
    let mut fb_file = File::open("/dev/fb0")?;
    
    let mut vinfo = std::mem::MaybeUninit::<FbVarScreeninfo>::uninit();
    unsafe {
        fbioget_vscreeninfo(fb_file.as_raw_fd(), vinfo.as_mut_ptr())?;
    }
    let vinfo = unsafe { vinfo.assume_init() };
    
    let width = vinfo.xres;
    let height = vinfo.yres;
    let width_virtual = vinfo.xres_virtual;
    let bpp = vinfo.bits_per_pixel;
    let bytes_per_pixel = (bpp / 8) as usize;
    
    let line_length = width_virtual as usize * bytes_per_pixel;
    let fb_size = line_length * height as usize;
    let mut fb_data = vec![0u8; fb_size];
    let _ = fb_file.read(&mut fb_data)?;
    
    let img: RgbaImage = ImageBuffer::from_fn(width, height, |x, y| {
        let idx = (y as usize * line_length) + (x as usize * bytes_per_pixel);
        
        if bpp == 32 {
            let b = fb_data.get(idx).cloned().unwrap_or(0);
            let g = fb_data.get(idx + 1).cloned().unwrap_or(0);
            let r = fb_data.get(idx + 2).cloned().unwrap_or(0);
            let _a = fb_data.get(idx + 3).cloned().unwrap_or(255);
            
            image::Rgba([r, g, b, 255])
        } else if bpp == 16 {
            let pixel = u16::from_le_bytes([fb_data[idx], fb_data[idx + 1]]);
            let r = ((pixel >> 11) & 0x1F) as u8;
            let g = ((pixel >> 5) & 0x3F) as u8;
            let b = (pixel & 0x1F) as u8;
            
            image::Rgba([
                (r << 3) | (r >> 2),
                (g << 2) | (g >> 4),
                (b << 3) | (b >> 2),
                255
            ])
        } else {
            image::Rgba([0, 0, 0, 255])
        }
    });

    let rotated = match rotation {
        Rotation::None => img,
        Rotation::Rotate90 => image::imageops::rotate90(&img),
        Rotation::Rotate180 => image::imageops::rotate180(&img),
        Rotation::Rotate270 => image::imageops::rotate270(&img),
    };
    
    rotated.save(output)?;

    Ok(())
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Args = argh::from_env();
    match fbscreenshot(&args.output, args.rotation) {
        Ok(()) => {
            println!("fbscreenshot: Saved screenshot to '{}'", &args.output);
            unsafe { nix::libc::_exit(0); }
        }
        Err(e) => {
            eprintln!("fbscreenshot: Failed saving screenshot to '{}': {e}", &args.output);
            exit(1)
        }
    }
}
