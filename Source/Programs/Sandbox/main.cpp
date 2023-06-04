#include "App/App.hpp"
#include "App/AppWindow.hpp"
#include "UI/UI.hpp"

class SandboxApp : public App {
    void init() override {
        auto button = new Button;
        auto window = new AppWindow(this, "XVEngine");
        window->setChild(button);
        addWindow(window);
    }

   public:
    SandboxApp() : App("SandboxApp") {}
};

int main(i32 argc, char** argv) {
    SandboxApp app;
    app.run();
}
