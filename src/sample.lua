-- A simple scene with five spheres

red = gr.material({1.0, 0.0, 0.0}, {0.5, 0.7, 0.5}, 25)
green = gr.material({0.0, 1.0, 0.0}, {0.5, 0.7, 0.5}, 25)
orange = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
blue = gr.material({0.0, 0.0, 1.0}, {0.5, 0.4, 0.8}, 25)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {100, 150, -200}, 50)
scene_root:add_child(s1)
s1:set_material(red)

b1 = gr.nh_box('b1', {120, -100, -200}, 50)
scene_root:add_child(b1)
b1:set_material(orange)

s2 = gr.nh_sphere('s2', {50, 150, -400}, 80)
scene_root:add_child(s2)
s2:set_material(green)

b2 = gr.nh_box('b2', {50, -150, -400}, 80)
scene_root:add_child(b2)
b2:set_material(blue)

s3 = gr.nh_sphere('s3', {-50, 150, -600}, 100)
scene_root:add_child(s3)
s3:set_material(blue)

b3 = gr.nh_box('b3', {-50, -200, -600}, 100)
scene_root:add_child(b3)
b3:set_material(green)

s4 = gr.nh_sphere('s4', {-200, 150, -900}, 150)
scene_root:add_child(s4)
s4:set_material(orange)

b4 = gr.nh_box('b4', {-200, -300, -900}, 150)
scene_root:add_child(b4)
b4:set_material(red)

white_light = gr.light({100.0, 100.0, 000.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({-50.0, 400.0, -600.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'sample.png', 256, 256,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
