#include "App/App.hpp"
#include "App/AppWindow.hpp"

class SandboxApp : public App {
    void init() override {
        auto window = new AppWindow(this, "XVEngine");
        addWindow(window);
    }

   public:
    SandboxApp() : App("SandboxApp") {}
};

int main(i32 argc, char** argv) {
    SandboxApp app;
    app.run();
}
