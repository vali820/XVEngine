#include "App/App.hpp"
#include "App/AppWindow.hpp"

class SandboxApp : public App {
    void init() override {
        auto window1 = new AppWindow(this, "Window1");
        addWindow(window1);
    }

   public:
    SandboxApp() : App("SandboxApp") {}
};

int main(i32 argc, char** argv) {
    SandboxApp app;
    app.run();
}
