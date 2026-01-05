use std::{
    fs, sync::{Arc, RwLock}, thread, time::{Duration, Instant},
};

mod config;
use config::{ProfileManager, watch_profiles};

mod thermalzone;
use thermalzone::ThermalZone;

const FAN_CONTROL_PATH: &str = "/sys/class/thermal/cooling_device0/cur_state";
const MAX_FAN_LEVEL: u8 = 31;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let manager = Arc::new(RwLock::new(ProfileManager::new()?));
    
    let manager_clone = Arc::clone(&manager);
    thread::spawn(move || {
        if let Err(e) = watch_profiles(manager_clone) {
            eprintln!("Profile watcher error: {}", e);
        }
    });
    
    let mut last_fan_level = fs::read_to_string(FAN_CONTROL_PATH)?.trim().parse::<u8>()?;
    let mut blocked_until = Instant::now();

    loop {
        let max_temp = ThermalZone::get_highest_temp()?;
        println!("temp: {:.2}°C, fan level: {}", (max_temp as f32 / 1000.0), last_fan_level);

        let mgr = manager.read().unwrap();
        let config = mgr.get_active_config()
            .ok_or("No active config")?;
        
        let fan_level = config.temp_to_fan_level(max_temp).clamp(0, MAX_FAN_LEVEL);
        let block_duration = config.block_duration_secs;
        let poll_interval = config.poll_interval_ms;

        let now = Instant::now();

        if fan_level != last_fan_level && (fan_level > last_fan_level || now >= blocked_until) {
            println!("new fan level: {:?}", &fan_level);
            fs::write(FAN_CONTROL_PATH, fan_level.to_string())?;
            last_fan_level = fan_level;
            blocked_until = now + Duration::from_secs(block_duration);
        }

        thread::sleep(Duration::from_millis(poll_interval));
    }
}
