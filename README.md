# VIGIL
![VIGILImage-10](https://user-images.githubusercontent.com/48212096/173429054-f3396d74-7f76-462c-8fd4-27e1634806fa.png)

VIGIL is a very short survival horror game with fixed camera angles drawing heavy inspiration from old survival horror games on the PS1 (mainly Resident Evil 1 but also a little bit of Silent Hill 1).

The premise is simple, you have just survived a car crash and have injured your leg. Your car is going nowhere and so you need to find help as quickly as possible as it’s starting to get dark out. After some walking, you notice other cars in similar disarray to yours which does not bode well for your current predicament. Eventually, you come across a mansion and make your way inside for sanctuary. Little did you know the horrors that awaited you inside…
The objective is simple: survive.

The game doesn't actually have an end/win state so you walk around until you die or kill all enemies. My wish to remake this game in a more matrure engine such as Unity, Godot, or Unreal Engine and take it further and expand the scope of the game.

# Mechanics
The player movement uses a tank controls system where you cannot turn while walking forward. You also cannot move backwards. These clunky controls can be a hindrance in some genres but in survival horror, if implemented effectively, can add to the tension in the game.

The game has incorporated in it an inventory and resource management system. Items you pick up in the world will be put into your inventory (which can be viewed by pressed Tab at any time). Inside the inventory, items can be reorganized and right-clicking on an item will pop up a context menu, allowing you to perform certain actions with that item.

The game uses fixed camera angles which harkens back to the old (survival) horror games like Silent Hill and Resident Evil.

Shooting mechanics are also implemented where the player can equip their handgun and shoot bullets which are handled by the physics manager. These bullets can deal damage to enemies.

# Controls
![VIGILImage-7](https://user-images.githubusercontent.com/48212096/173429126-114b11cc-9eb1-479f-aa1a-4245bf339548.png)

Apart from the controls listed in the game’s controls menu, there are some debugging controls:
  - **Q** will switch to a free cam mode. Moving around in free cam mode is done using **I J K L**.
  - **T** - view bounding boxes.
  - **Y** - view enemy a* pathfinding (in level 2).
  - There are also other controls commented out in the code which can be uncommented if desired. The fixed camera angles were created using these controls:
    - **ENTER** – Create a fixed camera angle that doesn’t follow the player
    - **\** - Create a fixed camera angle that follows the player.
    - **Backspace** – Delete selected fixed camera bounding box.
    - **Right Shift** – Select the camera angle that the free cam is currently colliding with.
    - **P** – Set the camera angle of the selected bounding box to the current view of the free cam.
    - **4** – Save current bounding boxes layout to file. WARNING: THIS WILL OVERWRITE THE CURRENT SETTINGS.
    - **Arrow Keys** – Move the selected bounding box.
    - **+/-** - Move up/down respectively.
    - **F1/1** – Increase/Decrease Width.
    - **F2/2** – Increase/Decrease Height.
    - **F3/3** – Increase/Decrease Depth.

# Build Instructions
Visual Studio 2022 is required for this project. Older visual studio version **might** work but I have only tested it with VS2022 so there is no guarentee. Along with that, **release mode does not work**, please build in **Debug** mode.

Simply open **engine.sln** in the root directory and click **Run Without Debugger**.

# Screenshots
![VIGILImage-1](https://user-images.githubusercontent.com/48212096/173429932-846b4c42-e380-4774-ae44-fc16202c8600.png)
![VIGILImage-2](https://user-images.githubusercontent.com/48212096/173429296-6617a1bd-11f2-499b-8c5f-8dd85dd371de.png)
![VIGILImage-3](https://user-images.githubusercontent.com/48212096/173429320-25e9282d-f98b-4aa1-89f9-9b9ba392ff5e.png)
![VIGILImage-4](https://user-images.githubusercontent.com/48212096/173429327-e83715e9-98ff-4ce2-a52c-fe1aed1ca521.png)
![VIGILImage-5](https://user-images.githubusercontent.com/48212096/173429331-34a96326-4aee-44a5-9571-ebf70d44ae53.png)
![VIGILImage-6](https://user-images.githubusercontent.com/48212096/173429335-e084c07d-4713-46f8-b098-c3da73ab056e.png)
![VIGILImage-8](https://user-images.githubusercontent.com/48212096/173429341-366d4ca0-af82-43b1-b574-39a2a62f13e6.png)
![VIGILImage-9](https://user-images.githubusercontent.com/48212096/173429348-5b0cc46c-f2e2-4cfb-9901-c03b068eda21.png)

# Assets
## Fonts
  -	**Main Menu Font:** https://fonts.google.com/specimen/IM+Fell+DW+Pica+SC?category=Serif&preview.text=Start%20Game&preview.text_type=custom
  -	**Generally used font:** https://fonts.google.com/specimen/Bebas+Neue?query=bebas
  -	**Generally used font 2:** https://www.freefonts.io/bebas-neue-bold-free-font/
  -	**Inventory Font:** https://fonts.google.com/specimen/Archivo+Narrow?query=archivo
## Meshes
  -	**Signs:** https://sketchfab.com/3d-models/warning-signs-set-2e8ef8f837c445578e3d08bdcd37ee3b
  -	**Player:** https://sketchfab.com/3d-models/male-character-ps1-style-78640adbd76b48129e9316259695c742
  -	**Enemies:** https://sketchfab.com/3d-models/polygonal-zombies-with-animations-free-pack-d9bcfdd88f5348549bc947226af7c314
  -	**Outdoor House:** https://sketchfab.com/3d-models/haunted-house-the-house-of-ancient-symbols-4673b5525dc24559af89106c60ebb5fb
  -	**Indoor House Level:** https://sketchfab.com/3d-models/haunted-house-f0300977ea9640959779453f8373591d
  -	**Handgun:** https://sketchfab.com/3d-models/m9-low-poly-958dfb2aa12e47a2af95dfc1a33263d4
  -	**Bullet:** https://sketchfab.com/3d-models/low-poly-bullet-pistol-6140f75bdedc4ceb804b478b71b133e6
## Textures
  -	**Main Menu Image:** https://unsplash.com/photos/pCgxm-HDMNs
  -	**Skybox:** https://opengameart.org/content/interstellar-skybox-png
  -	**Tree Texture:** https://www.textures.com/download/BarkDecidious0083/13110
  -	**First Aid Spray Background:** https://textures.com/download/PBR0186/133225
  -	**Ammo Box:** https://www.textures.com/download/PBR0050/133085
  -	**UI Dirty Background:** https://www.textures.com/download/Overlays0071/130459
  -	**Dirty Snow:** https://www.textures.com/download/3DScans0102/128413
## Audio
  -	**Main Menu Ambient Music:** https://freesound.org/people/ethanchase7744/sounds/439509/
  -	**Level 2 Ambient Music:** https://freesound.org/people/pfranzen/sounds/393808/
  -	**Gunshot Empty:** https://freesound.org/people/TampaJoey/sounds/588733/
  -	**Gunshot:** https://freesound.org/people/tehlordoswag420/sounds/249778/
  -	**Snow Footsteps:** https://freesound.org/people/Nox_Sound/sounds/490951/
  -	**Indoor Footsteps:** https://freesound.org/people/dkiller2204/sounds/366111/
  -	**UI Click:** https://freesound.org/people/unfa/sounds/565132/
  -	**UI Reveal:** https://freesound.org/people/newlocknew/sounds/515834/
  -	**UI Reveal 2:** https://freesound.org/people/steaq/sounds/577464/
