plum.setTitle('Giraffes IN SPACE')


img = plum.Image('explosiveavatar96.png')
tex = plum.Texture('resources/sprites/heartsprite.png')
fnt = plum.Font('resources/fonts/ccfont.png')
fnt:enableVariableWidth()

snd = plum.Sound('resources/sounds/shot.wav');

print("width = " .. tex.width .. "; height = " .. tex.height)

song = plum.Song('resources/songs/zk-sf-09-boss2.xm');
song:play()

time = plum.timer.time

x = 160; y = 120;
angle = 0;

while not plum.key.Escape.pressed do
    plum.video.verticalGradientRect(0, 0, plum.video.screenWidth, plum.video.screenHeight, plum.color.rgb(0x33, 0x66, 0xcc), plum.color.Magenta)

    img:blit(96 - math.random() * 192, 96 - math.random() * 192, tex.image, plum.blend.Opaque)
    tex:refresh()
    tex:rotateScaleBlitRegion(1, 1, 16, 16, x, y, angle, 3 + math.sin(math.rad(plum.timer.time)) * 0.25 )
    
    fnt:print(5, 5, "FPS: " .. plum.timer.fps .. " " .. tostring(plum.key.Enter.pressed))
    for i = 1, plum.timer.gap do
        angle = angle + 0.5
        
        if plum.key.Enter.pressed then
            snd:play()
            plum.key.Enter.pressed = false
        end
        
        if plum.key.Left.pressed then
            x = x - 1
        elseif plum.key.Right.pressed then
            x = x + 1
        end
        
        if plum.key.Up.pressed then
            y = y - 1
        elseif plum.key.Down.pressed then
            y = y + 1
        end
    end
    plum.refresh()
end

