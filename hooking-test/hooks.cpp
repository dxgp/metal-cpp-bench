#include <dlfcn.h>
#include <objc/runtime.h>
#include <objc/message.h>


int main(){
    void *mtl = dlopen("/System/Library/Frameworks/Metal.framework/Metal", RTLD_LAZY);
    id (*create_dev)(void) = (id (*)(void))dlsym(mtl, "MTLCreateSystemDefaultDevice");
    auto device = create_dev();
    return 0;
}

// void createMetalCommandQueue(){
//     *(void **) (&MTLCreateCommandQueue) = dlsym(mtpath, "MTLCreateCommandQueue");
// }
// void createMetalCommandBuffer(){
//     *(void **) (&MTLCommandQueueCommandBuffer) = dlsym(mtpath, "MTLCommandQueueCommandBuffer");
// }
// void createMetalCommandBufferCommit(){
//     *(void **) (&MTLCommandBufferCommit) = dlsym(mtpath, "MTLCommandBufferCommit");
// }
// void createMetalCommandBufferWaitUntilCompleted(){
//     *(void **) (&MTLCommandBufferWaitUntilCompleted) = dlsym(mtpath, "MTLCommandBufferWaitUntilCompleted");
// }
// void createMetalCommandBufferPresentDrawable(){
//     *(void **) (&MTLCommandBufferPresentDrawable) = dlsym(mtpath, "MTLCommandBufferPresentDrawable");
// }
// void createMetalCommandBufferAddCompletedHandler(){
//     *(void **) (&MTLCommandBufferAddCompletedHandler) = dlsym(mtpath, "MTLCommandBufferAddCompletedHandler");
// }
// void createMetalCommandBufferAddScheduledHandler(){
//     *(void **) (&MTLCommandBufferAddScheduledHandler) = dlsym(mtpath, "MTLCommandBufferAddScheduledHandler");
// }
// void createMetalCommandBufferAddCompletedHandlerAfterStages(){
//     *(void **) (&MTLCommandBufferAddCompletedHandlerAfterStages) = dlsym(mtpath, "MTLCommandBufferAddCompletedHandlerAfterStages");
// }
// void createMetalCommandBufferAddScheduledHandlerAfterStages(){
//     *(void **) (&MTLCommandBufferAddScheduledHandlerAfterStages) = dlsym(mtpath, "MTLCommandBufferAddScheduledHandlerAfterStages");
// }
// void createMetalCommandBufferPresentDrawableAtTime(){
//     *(void **) (&MTLCommandBufferPresentDrawableAtTime) = dlsym(mtpath, "MTLCommandBufferPresentDrawableAtTime");
// }
// void createMetalCommandBufferWaitUntilScheduled(){
//     *(void **) (&MTLCommandBufferWaitUntilScheduled) = dlsym(mtpath, "MTLCommandBufferWaitUntilScheduled");
// }
// void createMetalCommandBufferWaitUntilPresented(){
//     *(void **) (&MTLCommandBufferWaitUntilPresented) = dlsym(mtpath, "MTLCommandBufferWaitUntilPresented");
// }
