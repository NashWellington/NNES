#include "cmd.hpp"

void CMD::cmdLoop(std::shared_ptr<Input> input)
{
    using namespace std;
    string command = "";
    while (input->running)
    {
        cout << "NNES> ";
        getline(cin, command);

        bool valid = true; // Determines whether it's an invalid command

        // tokenize
        vector<string> cmd_tokens = {};
        while (command.length() > 0)
        {
            auto i = command.find(" ");
            if (i > 0) // Prevents filling vector w/ empty strings if there are multiple spaces in a row
            {
                cmd_tokens.push_back(command.substr(0, i));
            }
            if (i != std::string::npos)
            {
                command = command.substr(i+1, command.length());
            }
            else
            {
                command = "";
            }
        }

        if (cmd_tokens.size() < 1) valid = false;
        else if (cmd_tokens[0] == "help")
        {
            cout << "NNES commands:" << endl;
            cout << "   help                                display help menu (you are here)" << endl;
            // TODO add power cycling
            // cout << "   power                               power cycles the console (i.e. turns it off and on again)" << endl;
            // cout << "                                       note: this functions slightly differently from a reset" << endl;
            cout << "   reset                               resets the console" << endl;
            // TODO add ROM file re-insertion
            // cout << "   rom (rom filename)                  switches the current ROM with a new one and power cycles" << endl;
            cout << "   pause                               pauses/resumes the emulation" << endl;
            cout << "   mute                                mutes/unmutes the emulation" << endl;
            cout << "   [show | hide] fps                   toggle framerate display" << endl;
            cout << "   [show | hide] render [ms | percent] toggle render timer in ms/percent mode" << endl;
            cout << "   quit                                closes NNES" << endl;
        }
        // else if (command == "power") {}
        else if (cmd_tokens[0] == "reset")
        {
            cout << "Resetting console" << endl;
            input->reset();
        }
        else if (cmd_tokens[0] == "rom")
        {
            // TODO

        }
        else if (cmd_tokens[0] == "pause")
        {
            bool paused = input->pause();
            cout << (paused ? "Emulation paused" : "Emulation resumed") << endl;
        }
        else if (cmd_tokens[0] == "mute")
        {
            bool muted = input->mute();
            cout << (muted ? "Emulation muted" : "Emulation unmuted") << endl;
        }
        else if (cmd_tokens[0] == "fps" || (cmd_tokens.size() >= 2 && cmd_tokens[1] == "fps"))
        {
            if (cmd_tokens[0] == "show") 
            {
                cout << "Showing";
                input->toggle_fps(true);
            }
            else if (cmd_tokens[0] == "hide")
            {
                cout << "Hiding";
                input->toggle_fps(false);
            }
            else 
            {
                cout << (input->toggle_fps() ? "Showing" : "Hiding");
            }
            cout << " FPS counter" << endl;
        }
        else if (cmd_tokens[0] == "render" || (cmd_tokens.size() >= 2 && cmd_tokens[1] == "render"))
        {
            Video::RenderTimeDisplay mode;
            if (cmd_tokens.size() == 1) // "render"
                mode = input->toggle_render_time();
            else if (cmd_tokens.size() == 2)
            {
                if (cmd_tokens[0] == "show") // "show render"
                    mode = input->toggle_render_time(true);
                else if (cmd_tokens[0] == "hide") // "hide render"
                    mode = input->toggle_render_time(false);
                else if (cmd_tokens[0] == "render")
                {
                    if (cmd_tokens[1] == "ms") // "render ms"
                    {
                        mode = Video::RenderTimeDisplay::MS;
                        input->toggle_render_time(mode);
                    }
                    else if (cmd_tokens[1] == "percent") // "render percent"
                    {
                        mode = Video::RenderTimeDisplay::PERCENT;
                        input->toggle_render_time();
                    }
                    else valid = false;
                }
                else valid = false;
            }
            else if (cmd_tokens.size() >= 3)
            {
                if (cmd_tokens[0] == "show")
                {
                    if (cmd_tokens[2] == "ms") // "show render ms"
                    {
                        mode = Video::RenderTimeDisplay::MS;
                        input->toggle_render_time(mode);
                    }
                    else if (cmd_tokens[2] == "percent") // "show render percent"
                    {
                        mode = Video::RenderTimeDisplay::PERCENT;
                        input->toggle_render_time(mode);
                    }
                    else valid = false;
                }
                else valid = false;
            }
            if (valid)
            {
                if (mode == Video::RenderTimeDisplay::NO)
                {
                    cout << "Hiding frame render timer" << endl;
                }
                else
                {
                    cout << "Displaying frame render timer ";
                    if (mode == Video::RenderTimeDisplay::MS) cout << "in milliseconds";
                    else cout << "as a percentage of one frame";
                    cout << endl;
                }
            }
        }
        else if (cmd_tokens[0] == "quit" || cmd_tokens[0] == "exit")
        {
            // cout << "Goodbye :^)" << endl;
            input->quit();
        }
        else valid = false;
        
        if (!valid)
        {
            cout << "Invalid command: \"";
            for (uint i = 0; i < cmd_tokens.size(); i++)
            {
                cout << cmd_tokens[i];
                if (i < cmd_tokens.size()-1) cout << " ";
            }
            cout << "\"" << endl;
            cout << "Enter \"help\" for a list of commands" << endl;
        }
    }
}