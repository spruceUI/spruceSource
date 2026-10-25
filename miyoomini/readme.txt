docker build -t pyui_miyoomini .

mkdir output
docker run --rm -v ${PWD}/output:/output pyui_miyoomini cp set_monitor /output/
docker run --rm -v ${PWD}/output:/output pyui_miyoomini cp send_event /output/
docker run --rm -v ${PWD}/output:/output pyui_miyoomini cp getevent /output/
docker run --rm -v ${PWD}/output:/output pyui_miyoomini cp axp_read /output/

Charge screen frames (Mini Flip), from a spruceOS checkout:
pip install pillow
python3 make_charge_frames.py <spruceOS>/Themes/SPRUCE/skin_752x560 <spruceOS>/spruce/miyoomini/charge
