### Command Line Arguments:

A very basic argument decoder is implemented in the _Application_ class.
Arguments can but don't need to be separated by `/` or `-`.
The general syntax for arguments is: `argument=value`.
The value part is optional.

#### Some Examples:

`arg1` `/arg1` `-arg1` `--arg1`

`arg2=value` `/arg2=value` `-arg2=value` `--arg2=value`

#### Example code to check for an argument:
```C++
Helios::ApplicationCommandLineArgs args = Helios::Application::Get().GetSpecification().CommandLineArgs;
bool test1 = args.Check("arg");
```

#### Example code to get the value of an argument:
```C++
Helios::ApplicationCommandLineArgs args = Helios::Application::Get().GetSpecification().CommandLineArgs;
std::string test1 = args.Get("arg", "optional default value");
std::string test2 = args.Get("arg"); // test2 == "" if arg does not exist
```

### Reserved Command Line Arguments:

| Argument:       | Description: |
| ---             | --- |
| `force_opengl`  | Forces the rendering engine to use OpenGL. |
| `force_vulkan`  | Forces the rendering engine to use Vulkan. |
| `force_directx` | Forces the rendering engine to use DirectX. <br/> _(Note: Windows only!)_ |
| `force_metal`   | Forces the rendering engine to use Metal.  <br/> _(Note: MacOS only!)_ |

