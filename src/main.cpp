#include <renderer/renderer.hpp>

using namespace std;

int main(int argc, char* argv[])
{
    /* Ask the user for a scene file if not passed. */
    string sceneFile;
    if (argc > 3) sceneFile = argv[1]; else
    {
        cout << "[+] Scene file to render: ";
        getline(cin, sceneFile);
    }

    /* Ask the user for an output file, if not passed. */
    string renderFile;
    if (argc > 3) renderFile = argv[2]; else
    {
        cout << "[+] Output file: ";
        getline(cin, renderFile);
    }

    /* Ask the user for a number of threads if unspecified. */
    size_t threadCount;
    if (argc > 3) threadCount = atoi(argv[3]); else
    {
        cout << "[+] Thread count: ";
        cin >> threadCount;
    }

    /* Line break (this is just for aesthetics). */
    if (argc <= 3) cout << endl;

    /* Initialize the renderer. */
    Renderer* renderer = new Renderer(sceneFile);

    /* Render the scene. */
    renderer->Render(renderFile, threadCount);

    /* Free everything. */
    delete renderer;
    return 0;
}
