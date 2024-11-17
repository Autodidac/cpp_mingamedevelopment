#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>

using namespace std::chrono_literals;

// Utility to get the last write time of a file
std::filesystem::file_time_type get_last_write_time(const std::filesystem::path& file) {
    return std::filesystem::last_write_time(file);
}

// Check for file modifications
bool is_file_modified(const std::filesystem::path& file, std::filesystem::file_time_type& last_write_time) {
    auto current_time = get_last_write_time(file);
    if (current_time != last_write_time) {
        last_write_time = current_time;
        return true;
    }
    return false;
}

// GameObject struct
struct GameObject {
    std::string name;
    float x, y, z;

    void move(float dx, float dy, float dz) {
        x += dx;
        y += dy;
        z += dz;
    }

    std::string to_string() const {
        return "Name: " + name + ", Position: (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
};

// Compile C# script using Roslyn (dotnet build)
bool compile_csharp_script(const std::filesystem::path& script_dir) {
    std::string command = "dotnet build " + script_dir.string() + "\\Script.csproj"; // Point to the .csproj file
    int result = std::system(command.c_str());
    return result == 0;
}

// Run the compiled C# code using Roslyn execution
void run_roslyn_script(const std::filesystem::path& script_dir, const GameObject& game_obj) {
    std::cout << "Executing C# script: " << script_dir << std::endl;

    // Setup the command to execute the C# script using dotnet run
    std::string command = "dotnet run --project " + script_dir.string() + "\\Script.csproj";

    // Here we can append arguments for the game object state if necessary
    std::string game_object_data = game_obj.to_string();
    command += " " + game_object_data; // Optionally pass game object data as an argument

    // Run the script with the provided game object data
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to execute C# script." << std::endl;
    }
}

int main() {
    try {
        // Example game object
        GameObject player = { "Player1", 0.0f, 0.0f, 0.0f };

        // Path to your C# script directory
        std::filesystem::path script_dir = ".\\"; // Adjust path to where the Script.cs and Script.csproj are located

        std::cout << "Script Directory: " << script_dir << std::endl;

        if (!std::filesystem::exists(script_dir)) {
            std::cerr << "Error: Script directory does not exist: " << script_dir << std::endl;
            return -1;
        }

        std::filesystem::file_time_type last_write_time = get_last_write_time(script_dir / "Script.cs");

        while (true) {
            if (is_file_modified(script_dir / "Script.cs", last_write_time)) {
                std::cout << "Change detected in script. Reloading..." << std::endl;

                // Compile the C# script using Roslyn
                if (compile_csharp_script(script_dir)) {
                    // Run the compiled C# script with the current game state
                    run_roslyn_script(script_dir, player);
                }
                else {
                    std::cerr << "Failed to compile script." << std::endl;
                }
            }

            std::this_thread::sleep_for(1s);  // Check every second
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
