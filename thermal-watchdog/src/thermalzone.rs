use std::fs;

#[derive(Debug)]
pub enum ThermalZone {
    LittleCPU,
    BigCPU,
    GPU,
}

impl ThermalZone {
    fn as_int(&self) -> u8 {
        match self {
            ThermalZone::LittleCPU => 0,
            ThermalZone::BigCPU => 1,
            ThermalZone::GPU => 2,
        }
    }

    pub fn get_temp(&self) -> Result<i32, Box<dyn std::error::Error>> {
        Ok(fs::read_to_string(format!(
            "/sys/class/thermal/thermal_zone{}/temp",
            self.as_int()
        ))?
        .trim()
        .parse::<i32>()?)
    }

    pub fn get_highest_temp() -> Result<i32, Box<dyn std::error::Error>> {
        Ok(*[
            ThermalZone::BigCPU.get_temp()?,
            ThermalZone::LittleCPU.get_temp()?,
            ThermalZone::GPU.get_temp()?,
        ]
        .iter()
        .max()
        .ok_or("Unable to get max temp")?)
    }
}
