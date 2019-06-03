# Fast-Belt

## Introduction

Procedural content generation game will initialize hundreds of thousands of entities in the scene. Different to web applications, where only a small subset of entities will be update each frame, in the game, all of hundreds of thousands of entities need to be update in every 16ms.

So there is a strong need in game system to parallelize object update. But it is not straightforward to do it, since entities may interact with each other, thus requires some preprocessing of object states.

In this project we build a simple world with 500000 box entities and 10 crossing belts, where boxes moves on the belt, and will stop moving to prevent collision if there is a "traffic jam" in front of it. We make position update and collision resolving logic parallel.

## Box Behavior

Belts (传送带) move boxes on them, boxes may collide with each other if belts are crossing.

Upon colliding, smart boxes may wait a second to prevent collision. Lots of objects to update in each frame of game loop.

### steps in a game frame

Foreach box, detect whether they may collapse with boxes on the other belt (near the intersection point of belt), if so, add a flag #nearIntersection to it.

This is done in parallel, then foreach intersection, choose one belt to active (diagram 1):

![diagram 1](doc/1.png)

Foreach box check if it is too close to the box in front of it.
When "the box in front of it" is waiting for traffic light, This is done in parallel, So items on the belt won’t stop all together immediately, instead works like a traffic jam (stop one box in each frame), see (diagram 2):

![diagram 2](doc/2.png)

Update boxes’ position based on context state and delta time in game loop, This is also done in parallel (diagram 3):

![diagram 3](doc/3.png)

## The MPI Approach

First we try to use MPI with 