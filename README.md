# Student Attendance Management System

## Overview
The **Student Attendance Management System** is a robust C++ console application designed to streamline the process of tracking student attendance, managing student records, and generating performance reports. Version 3.0 introduces a comprehensive Student Portal alongside the existing Admin and Teacher modules.

## Features
- **Role-Based Access Control**:
  - **Admin**: Full control over student/teacher records, system settings, and data management.
  - **Teacher**: Mark attendance, view student lists, and generate reports.
  - **Student**: View personal attendance records, exam results, and profile details.
- **Attendance Tracking**: Record daily attendance with support for different subjects.
- **Reporting System**:
  - Generate CSV reports for full attendance, daily summaries, department-wise statistics, and monthly individual records.
  - Student performance reports based on exam results.
- **Data Security**: Basic encryption for user passwords.
- **Data Persistence**: All data is saved to text files (`students.txt`, `attendance.txt`, etc.) for persistence between sessions.
- **Cross-Platform**: Designed to compile and run on both Windows and Linux systems.

## Prerequisites
- A C++ Compiler (GCC/G++ recommended)
- Windows OS (preferred for UI features) or Linux

## Compilation & Usage

### Windows
1.  Open your terminal or command prompt.
2.  Navigate to the project directory.
3.  Compile the source code:
    ```cmd
    g++ student6.cpp -o student_system.exe
    ```
4.  Run the application:
    ```cmd
    student_system.exe
    ```

### Linux
1.  Open your terminal.
2.  Navigate to the project directory.
3.  Compile the source code:
    ```bash
    g++ student6.cpp -o student_system
    ```
4.  Run the application:
    ```bash
    ./student_system
    ```

## File Structure
- `student6.cpp`: Main source code file.
- `students.txt`: Database of student records.
- `attendance.txt`: Database of attendance records.
- `admin.txt`, `teacher.txt`: Credentials for admins and teachers.
- `exam_results.txt`: Storage for exam scores and grades.

## Author
Developed by **Maaj Nandoliya**
