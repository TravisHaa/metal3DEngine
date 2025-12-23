//source file!
#include "app_delegate.h"
AppDelegate::~AppDelegate()
{
    delete viewDelegate;
    mtkView->release();
    window->release();
    device->release();
    
}

//make the gui app
void AppDelegate::applicationWillFinishLaunching(NS::Notification* notification)
{
    NS::Application* app = reinterpret_cast<NS::Application*>(notification->object());
    app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void AppDelegate::applicationDidFinishLaunching(NS::Notification* notification)
{
    //{x,y}, {width, height} of window frame
    CGRect frame = (CGRect){ {100.0, 100.0}, {640.0, 480.0} };

    window = NS::Window::alloc()->init(
        frame,
        NS::WindowStyleMaskClosable|NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false);
    //create metal device, finds best GPU and returns the device to create command q, buffers, and pipelines
    device = MTL::CreateSystemDefaultDevice();
    //create metalkit view, handles textures and display timing
    mtkView = MTK::View::alloc()->init(frame, device);
    //configure render properties, BGRA8 is the standard macOS display format
    mtkView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    mtkView->setClearColor(MTL::ClearColor::Make(1.0, 1.0, 0.0, 1.0));
    //attach a render delegate
    //MTK::View
//    └── calls ViewDelegate every frame
//            └── issues Metal draw commands
    viewDelegate = new ViewDelegate(device);
    mtkView->setDelegate(viewDelegate);
    //metal view becomes windows content
    window->setContentView(mtkView);
    window->setTitle(NS::String::string("Window", NS::StringEncoding::UTF8StringEncoding));
    window->makeKeyAndOrderFront(nullptr);
    
    //activate app
    NS::Application* app = reinterpret_cast<NS::Application*>(notification->object());
    app->activateIgnoringOtherApps(true);
}

bool AppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application* sender)
{
    return true;
}
//big picture

//NS::Application
//   └── AppDelegate
//         ├── NS::Window
//         │     └── MTK::View
//         │           └── ViewDelegate
//         │                 └── Metal rendering code
//         └── MTL::Device
