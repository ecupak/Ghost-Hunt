# Ghost-Hunt
Look for some hidden ghosts!

# How To Play
Left-click to start the game. Up to 3 ghosts are hiding in the static.
Move your mouse around to find them with your scope. Once found, they'll stick around!
After you find them all, they'll want to play again.

# Why
This was an elaborate test on working with pixels and implementing opacity. I got carried away as usual, but it was fun.

# How
The ghosts are drawn on a separate surface with the same dimensions as the visible surface.
When the scope overlaps a ghost's coordinates, the overlapping part of the ghost is copied to the visible surface.
But first it has *opacity* applied to it. Pixel = ghost_pixel * opacity + destination_pixel * (1 - opacity).

A separate image is used to provide the opacity values for the scope. Higher opacity towards the center.
Because I don't have any imaging software that can actually save images with partial opacity, I made up my own system.
Every color on the opacity-map is between 0x0000FF and 0x000000. I use the last 2 bytes/8 bits of data to determine the opacity level.

Sometimes the ghost takes too long when finding a random spot to generate at (either constantly being put too close to the edge or
overlapping another ghost), so that's why some games have only 2 ghosts present instead of 3. I haven't seen a game with just 1 ghost yet.
