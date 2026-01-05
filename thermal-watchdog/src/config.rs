use inotify::{Inotify, WatchMask};
use serde::{Deserialize, Serialize};
use std::{
    fs, collections::HashMap, path::PathBuf, sync::{Arc, RwLock}
};

const PROFILES_DIR: &str = "/mnt/SDCARD/spruce/etc/thermal-watchdog/profiles";
const ACTIVE_PROFILE_PATH: &str = "/mnt/SDCARD/spruce/etc/thermal-watchdog/active_profile";

#[derive(Debug, Deserialize, Serialize)]
pub struct FanCurvePoint {
    pub temp: i32,
    pub fan_level: u8,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Config {
    #[serde(default = "default_poll_interval")]
    pub poll_interval_ms: u64,
    
    #[serde(default = "default_block_duration")]
    pub block_duration_secs: u64,
    
    pub fan_curve: Vec<FanCurvePoint>,
}

fn default_poll_interval() -> u64 { 500 }
fn default_block_duration() -> u64 { 10 }

impl Config {
    pub fn load(path: &PathBuf) -> Result<Self, Box<dyn std::error::Error>> {
        let content = fs::read_to_string(path)?;
        Ok(serde_json::from_str(&content)?)
    }

    pub fn temp_to_fan_level(&self, temp: i32) -> u8 {
        for point in &self.fan_curve {
            if temp >= point.temp {
                return point.fan_level;
            }
        }
        0
    }
}

pub struct ProfileManager {
    profiles: HashMap<String, Config>,
    active_profile: String,
}

impl ProfileManager {
    pub fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let mut manager = ProfileManager {
            profiles: HashMap::new(),
            active_profile: String::new(),
        };
        
        manager.load_all_profiles()?;
        manager.load_active_profile()?;
        
        Ok(manager)
    }
    
    pub fn load_all_profiles(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        self.profiles.clear();
        
        for entry in fs::read_dir(PROFILES_DIR)? {
            let path = entry?.path();
            
            if path.extension().and_then(|s| s.to_str()) == Some("json") {
                if let Some(profile_name) = path.file_stem().and_then(|s| s.to_str()) {
                    match Config::load(&path) {
                        Ok(config) => {
                            println!("loaded profile: {}", profile_name);
                            self.profiles.insert(profile_name.to_string(), config);
                        }
                        Err(e) => {
                            eprintln!("failed to load profile {}: {}", profile_name, e);
                        }
                    }
                }
            }
        }
        
        if self.profiles.is_empty() {
            return Err("no valid profiles found".into());
        }
        
        Ok(())
    }
    
    pub fn load_active_profile(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        let active = fs::read_to_string(ACTIVE_PROFILE_PATH)?
            .trim()
            .to_string();
        
        if !self.profiles.contains_key(&active) {
            return Err(format!("active profile '{}' not found", active).into());
        }
        
        self.active_profile = active;
        println!("active profile: {}", self.active_profile);
        
        Ok(())
    }
    
    pub fn get_active_config(&self) -> Option<&Config> {
        self.profiles.get(&self.active_profile)
    }
}

pub fn watch_profiles(manager: Arc<RwLock<ProfileManager>>) -> Result<(), Box<dyn std::error::Error>> {
    let mut inotify = Inotify::init()?;
    
    inotify.watches().add(
        PROFILES_DIR,
        WatchMask::MODIFY | WatchMask::CLOSE_WRITE | WatchMask::CREATE | WatchMask::DELETE,
    )?;
    
    inotify.watches().add(
        ACTIVE_PROFILE_PATH,
        WatchMask::CLOSE_WRITE,
    )?;
    
    let mut buffer = [0; 4096];
    
    loop {
        let events = inotify.read_events_blocking(&mut buffer)?;
        
        for event in events {
            let is_modify = event.mask.contains(inotify::EventMask::MODIFY) 
                || event.mask.contains(inotify::EventMask::CLOSE_WRITE);
            let is_create_delete = event.mask.contains(inotify::EventMask::CREATE)
                || event.mask.contains(inotify::EventMask::DELETE);
            
            if is_modify || is_create_delete {
                println!("profiles directory changed: reloading...");
                
                let mut mgr = manager.write().unwrap();
                
                if is_create_delete || event.name.is_some() {
                    if let Err(e) = mgr.load_all_profiles() {
                        eprintln!("failed to reload profiles: {}", e);
                        continue;
                    }
                }
                
                if let Err(e) = mgr.load_active_profile() {
                    eprintln!("failed to reload active profile: {}", e);
                }
            }
        }
    }
}
