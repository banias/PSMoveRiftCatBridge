# PSMoveRiftCatBridge
Is a client to [PSMoveService](https://github.com/cboulay/PSMoveService) that allows to connect PS Move Controllers to PC and track thier postion using multiple cameras. PSMoveRiftCatBridge is an alternative to [PSMoveFreepieBridge](https://bitbucket.org/hawkinse/psmovefreepiebridge) when using PSMoveService with RiftCat/VRidge.
Instead of using FreePie to send the position data to RfitCat it uses the [VRidge API](https://github.com/RiftCat/vridge-api)

NOTE!
The project is currently in a *very very early proof-of-concetp stage*. This is why currently there is no configuration. Nonetheless any feedback would be appreciated :)

## Features
* Position tracking using one controller 
* Recenter - in order to recenter view in RiftCat push SELECT + START button on the controller that is used for head trakcking

## Usage 
Filter the Controller 0 in the steamvr.vrsettings (PSMoveRiftCatBridge assumes that controller 0 is used for head tracking)
```
  "psmove_settings": {
    "psmove_filter_hmd_serial": "XX:XX:XX:XX:XX:XX"
  },
```
Where `XX:XX:XX:XX:XX:XX` is the DeviceId of controller 0.  
1. Start PSMoveSerivce
2. Start RiftCat and pair it with your phone 
3. Start SteamVR
4. Start PSMoveRiftCatBridge

## Building from sources
All you need is a VS 2017 with C++/CLI SDK. 

## Attribution
The client code is heavily based on [PSMoveFreePieBridge](https://bitbucket.org/hawkinse/psmovefreepiebridge) code and client code examples from [PSMoveService](https://github.com/cboulay/PSMoveService).
