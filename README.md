# Student Course Prerequisite Checker

A C++ console-based academic planning application that models university courses, prerequisite relationships, degree requirements, and semester scheduling.

> **University of Nairobi — 2nd Year Project Assignment**

## Overview

The Student Course Prerequisite Checker was developed as a second-year project assignment at the **University of Nairobi**. It demonstrates how object-oriented programming, graph algorithms, and academic planning concepts can be combined to solve a practical problem faced by university students: understanding course dependencies and planning a realistic path toward graduation.

The application provides an interactive Windows console experience for exploring courses, checking prerequisites, reviewing an academic plan, validating graduation requirements, and generating an optimized course schedule.

## Why This Project Matters

Course planning can become complicated when students must account for prerequisite chains, completed coursework, semester availability, credit requirements, and graduation milestones. This project turns those relationships into a structured, navigable model and provides actionable planning utilities through a menu-driven interface.

It highlights practical software engineering skills including:

- Object-oriented design in modern C++
- Graph representation and traversal
- Depth-first and breadth-first search
- Topological sorting for dependency-aware scheduling
- Requirement validation and academic planning
- Defensive input and error handling
- Building a user-focused command-line application

## Features

### Course and prerequisite management

- View the complete course catalogue
- Add new courses interactively
- Add prerequisite relationships between courses
- Display courses, credit values, offering semesters, and prerequisites
- Detect cycles in the prerequisite graph

### Academic planning

- Track completed courses and grades
- Calculate an overall GPA for completed coursework
- Organize planned courses across eight semesters
- View the current academic plan
- Identify courses currently available based on completed prerequisites

### Path and dependency analysis

- Find the shortest path between courses
- Find all available paths through the prerequisite graph
- Identify all prerequisite courses required for a target course
- Render prerequisite structures as text and ASCII trees

### Graduation and scheduling support

- Define course, elective, category, credit, and compound requirements
- Check whether an academic plan satisfies graduation requirements
- Report missing requirements
- Generate an eight-semester schedule using prerequisite-aware ordering
- Respect configurable maximum courses per semester

## Technical Design

The application is implemented as a single C++ source file, `main.cpp`, and is organized around focused classes:

| Component | Responsibility |
| --- | --- |
| `Course` | Stores course identity, title, credits, topics, and offering semesters |
| `Requirement` | Represents course, elective, category, credit, and compound requirements |
| `PrerequisiteGraph` | Models course dependencies and postrequisite relationships |
| `PathFinder` | Finds prerequisite chains and shortest/all paths |
| `AcademicPlan` | Tracks completed courses, grades, GPA, and planned semesters |
| `GraduationValidator` | Checks degree requirements and identifies missing requirements |
| `ScheduleAssistant` | Produces a dependency-aware academic schedule |
| `Visualizer` | Displays prerequisite relationships as text and ASCII trees |

## Algorithms and Data Structures

- **Directed graph:** represents prerequisite and postrequisite relationships
- **Depth-first search (DFS):** supports cycle detection, path enumeration, and prerequisite discovery
- **Breadth-first search (BFS):** finds shortest paths between courses
- **Topological sorting:** orders courses while respecting dependency constraints
- **Hash maps and sets:** provide efficient course lookup and visited-node tracking
- **Vectors, stacks, queues, and maps:** support planning, traversal, and scheduling workflows

## Getting Started

### Prerequisites

- Windows, Linux, or macOS
- A C++ compiler with C++11 or later support, such as:
  - MinGW-w64 / g++
  - Microsoft Visual C++
  - Clang

### Clone the repository

```bash
git clone https://github.com/JosephOmollo/Student_Course_Prerequisite_Checker.git
cd Student_Course_Prerequisite_Checker
```

### Build with g++

```bash
g++ -std=c++11 -Wall -Wextra -o StudentCoursePrerequisiteChecker main.cpp
```

On Windows, this produces `StudentCoursePrerequisiteChecker.exe` when using MinGW.

### Run the application

Windows Command Prompt:

```cmd
StudentCoursePrerequisiteChecker.exe
```

Linux or macOS:

```bash
./StudentCoursePrerequisiteChecker
```

## Application Menu

When launched, the program provides options to:

1. View all courses
2. View the prerequisite structure
3. View the academic plan
4. Check the graduation path
5. Find a path between courses
6. Generate an optimal schedule
7. View available courses
8. Visualize a prerequisite tree
9. Add a new course
10. Add a prerequisite
11. Exit

## Sample Course Model

The demonstration data includes courses such as:

- `CS101` — Introduction to Programming
- `CS201` — Data Structures
- `CS202` — Algorithms
- `CS301` — Database Systems
- `CS320` — Operating Systems
- `CS401` — Software Engineering
- `CS410` — Artificial Intelligence
- `CS430` — Machine Learning
- `MATH101` — Calculus I
- `MATH202` — Linear Algebra

For example, the application models relationships such as:

```text
CS101 → CS201 → CS202 → CS410 → CS430
```

This allows the system to determine prerequisite chains and schedule courses in a dependency-aware order.

## Project Context

This project was completed as a **second-year University of Nairobi project assignment**. It represents an academic implementation of a real-world planning problem and reflects my development in:

- Software design and abstraction
- Algorithmic problem solving
- C++ programming
- Data structures and graph theory
- Translating requirements into working functionality

## Future Improvements

Potential next steps include:

- Separating classes into header and implementation files
- Adding persistent storage for courses and student plans
- Introducing automated unit and integration tests
- Improving input validation and user feedback
- Adding a graphical user interface for Windows
- Supporting multiple degree programmes and configurable curricula
- Exporting schedules and academic plans to files
- Adding richer credit and category-based graduation calculations

## Author

**Joseph Omollo**  
Electrical and Electronics Engineering
University of Nairobi student and aspiring software developer

- GitHub: [JosephOmollo](https://github.com/JosephOmollo)
- Repository: [Student Course Prerequisite Checker](https://github.com/JosephOmollo/Student_Course_Prerequisite_Checker)

## License

No license has been specified for this repository. Please contact the author before redistributing or reusing the code commercially.
