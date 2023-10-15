# d3d7batch
DLL wrapper for DrawPrimitive Batching. 

It is targeted at improving Ground Control, as there is a huge overhead in wine
 for each `DrawPrimitive()` call.

* BUG https://bugs.winehq.org/show_bug.cgi?id=33814
* Wine patch https://gitlab.winehq.org/wine/wine/-/merge_requests/2105
## Compile

```sh
sudo apt install make gcc-mingw-w64-i686
make
```

## Install

0. compile `ddraw.dll`
1. drop the `ddraw.dll` into the same folder as `gc.exe`.
2. do `export WINEDLLOVERRIDES="ddraw=n,b"`
3. run `wine gc.exe`
4. enjoy faster FPS
