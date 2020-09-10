#include <windows.h>
#include <stdio.h>
#include <commctrl.h>

#define MAX_NUM_ICONS 512
#define MAX_NUM_CHARS_PER_LINE 10

int main(int argc, char* args[]) {
    if (argc != 2) {
        printf("Please provide coordinate file name as an argument.\n");
        printf("Usage: '%s coordinate_file_name'\n", args[0]);
        printf("args: %s", args[1]);
        return 1;
    }
    
    HANDLE file;
    file = CreateFile(args[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        printf("Unable to open file '%s'.\n", args[1]);
    }
    
    DWORD read_buffer_size = MAX_NUM_ICONS*MAX_NUM_CHARS_PER_LINE;
    char read_buffer[MAX_NUM_ICONS*MAX_NUM_CHARS_PER_LINE] = {};
    DWORD num_bytes_read = 0;
    bool read_success = ReadFile(file, &read_buffer, read_buffer_size - 1, &num_bytes_read, NULL);
    if (!read_success) {
        printf("Read from file '%s' failed. Exiting...\n", args[1]);
        return 2;
    }
    
    int x_coords[MAX_NUM_ICONS] = {};
    int y_coords[MAX_NUM_ICONS] = {};
    int coords_index = 0;
    
    char* cursor = &read_buffer[0];
    char str_buffer[MAX_NUM_CHARS_PER_LINE] = {};
    int str_len = 0;
    bool parsing_x = true;
    
    while (cursor < &read_buffer[0] + num_bytes_read) {
        
        while (parsing_x) {
            if (*cursor == ',') {
                parsing_x = false;
                str_buffer[str_len] = '\0';
                x_coords[coords_index] = atoi(str_buffer);
                str_len = 0;
            }
            else {
                str_buffer[str_len] = *cursor;
                str_len += 1;
            }
            cursor += 1;
        }
        
        while (!parsing_x) {
            if (*cursor == '\n' || *cursor == '\0') {
                parsing_x = true;
                str_buffer[str_len] = '\0';
                y_coords[coords_index] = atoi(str_buffer);
                str_len = 0;
            }
            else {
                str_buffer[str_len] = *cursor;
                str_len += 1;
            }
            cursor += 1;
        }
        
        printf("Icon %d - x: %d, y: %d\n", coords_index, x_coords[coords_index], y_coords[coords_index]);
        
        coords_index += 1;
    }
    
    CloseHandle(file);
    
    LPCSTR shell_name = "SHELLDLL_DefView";
    LPCSTR list_view_name = "SysListView32";
    
    HWND hShellWnd = GetShellWindow();
    HWND hDefView = FindWindowEx(hShellWnd, NULL, shell_name, NULL);
    HWND desktop_list_view = FindWindowEx(hDefView, NULL, list_view_name, NULL);
    
    int icon_index = 0;
    int x = x_coords[icon_index];
    int y = y_coords[icon_index];
    
    while (ListView_SetItemPosition(desktop_list_view, icon_index, x, y)) {
        icon_index += 1;
        x = x_coords[icon_index];
        y = y_coords[icon_index];
        
        if (x == 0 && y == 0) {
            printf("Done! %d icons processed.\n", icon_index);
            break;
        }
    }
    
    return 0;
}
