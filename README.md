## Shard++
The project is about rewriting the game-engine "Shard" supplied in the course "TDA572 / DIT572 Game engine architecture" from C# to C++.

**Some features:**
- Cubemap (without environment mapping)
- Basic shading (Blinn-Phong lighting)
- 3D model loading with materials that can be used for physically-based rendering (PBR)
- AABB collision boxes for game objects
- Sweep and prune (SAP) collision detection in 3D
- Event listeners for game objects (e.g. OnCollisionEnter, OnCollisionStay, OnCollisionExit)
- Flight controls (euler angles, pitch, yaw, roll) for the player (which is a spaceship)
- First person camera
- Possible to set your own default shader
- Possible to click on screen and check if object is clicked
- Custom environment mapping
- Heightfields and procedurally generated terrain 
- Pathtracing and the ability to change renderer runtime
