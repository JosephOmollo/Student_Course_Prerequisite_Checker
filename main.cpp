
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <functional>
#include <map>
#include <limits>


// Forward declarations
class Course;
class Requirement;
class AcademicPlan;
class PrerequisiteGraph;
class PathFinder;
class GraduationValidator;
class ScheduleAssistant;
class Visualizer;

 //Course class to represent a university course
class Course {
private:
    std::string courseId;
    std::string name;
    int credits;
    std::vector<std::string> topics;
    int semesterOffered; // Bitmap: 1=Spring, 2=Summer, 4=Fall (can be combined)

public:
    Course() : courseId(""), name(""), credits(0), semesterOffered(0) {}
    Course(const std::string& id, const std::string& courseName, int courseCredits, int offeredSemester)
        : courseId(id), name(courseName), credits(courseCredits), semesterOffered(offeredSemester) {}

    std::string getId() const { return courseId; }
    std::string getName() const { return name; }
    int getCredits() const { return credits; }
    
    void addTopic(const std::string& topic) {
        topics.push_back(topic);
    }
    
    bool isOfferedInSemester(int semester) const {
        return (semesterOffered & semester) != 0;
    }
    
    std::string getSemestersString() const {
        std::string result;
        if (semesterOffered & 1) result += "Spring ";
        if (semesterOffered & 2) result += "Summer ";
        if (semesterOffered & 4) result += "Fall ";
        return result;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << courseId << " - " << name << " (" << credits << " credits)";
        return ss.str();
    }
};

// Requirement class for degree requirements

class Requirement {
public:
    enum Type {
        COURSE,          // Specific course
        ONE_OF,          // One course from a list
        COURSE_CATEGORY, // Courses from a specific category
        CREDIT_COUNT,    // Number of credits in a category
        COMPOUND         // Combination of other requirements
    };

private:
    std::string name;
    Type type;
    std::vector<std::string> courseOptions; // For ONE_OF or COURSE type
    std::string category;                   // For COURSE_CATEGORY
    int creditRequirement;                  // For CREDIT_COUNT
    std::vector<Requirement> subRequirements; // For COMPOUND

public:
    Requirement(const std::string& requirementName, Type requirementType) : name(requirementName), type(requirementType), creditRequirement(0) {}
    
    std::string getName() const { return name; }
    Type getType() const { return type; }
    
    void addCourseOption(const std::string& courseId) {
        courseOptions.push_back(courseId);
    }
    
    void setCourseCategory(const std::string& cat) {
        category = cat;
    }
    
    void setCreditRequirement(int credits) {
        creditRequirement = credits;
    }
    
    void addSubRequirement(const Requirement& req) {
        subRequirements.push_back(req);
    }
    
    std::vector<std::string> getCourseOptions() const { return courseOptions; }
    std::string getCategory() const { return category; }
    int getCreditRequirement() const { return creditRequirement; }
    const std::vector<Requirement>& getSubRequirements() const { return subRequirements; }
    
    std::string toString() const {
        std::stringstream ss;
        ss << name << " (";
        
        switch(type) {
            case COURSE:
                ss << "Course: " << courseOptions[0];
                break;
            case ONE_OF:
                ss << "One of: ";
                for (size_t i = 0; i < courseOptions.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << courseOptions[i];
                }
                break;
            case COURSE_CATEGORY:
                ss << "Category: " << category;
                break;
            case CREDIT_COUNT:
                ss << creditRequirement << " credits in " << category;
                break;
            case COMPOUND:
                ss << "Compound with " << subRequirements.size() << " sub-requirements";
                break;
        }
        
        ss << ")";
        return ss.str();
    }
};

 // PrerequisiteGraph implements a directed graph to represent course prerequisites

class PrerequisiteGraph {
private:
    std::unordered_map<std::string, std::vector<std::string>> prerequisites;
    std::unordered_map<std::string, std::vector<std::string>> postrequisites;
    std::unordered_map<std::string, Course> courses;

public:
    void addCourse(const Course& course) {
        courses[course.getId()] = course;
        if (prerequisites.find(course.getId()) == prerequisites.end()) {
            prerequisites[course.getId()] = std::vector<std::string>();
        }
        if (postrequisites.find(course.getId()) == postrequisites.end()) {
            postrequisites[course.getId()] = std::vector<std::string>();
        }
    }
    
    void addPrerequisite(const std::string& course, const std::string& prereq) {
        // Ensure both courses exist in the graph
      if (!hasCourse(course)) {
        std::cerr << "[WARN] Cannot add prerequisite: Course " << course << " does not exist.\n";
        return;
    }
    if (!hasCourse(prereq)) {
        std::cerr << "[WARN] Cannot add prerequisite: Prerequisite " << prereq << " does not exist.\n";
        return;
    }
        // Add the prerequisite relationship
        prerequisites[course].push_back(prereq);
        postrequisites[prereq].push_back(course);
    }
    
    bool hasPrerequisite(const std::string& course, const std::string& prereq) const {
        auto it = prerequisites.find(course);
        if (it == prerequisites.end()) return false;
        
        return std::find(it->second.begin(), it->second.end(), prereq) != it->second.end();
    }
    
    const std::vector<std::string>& getPrerequisites(const std::string& courseId) const {
        static const std::vector<std::string> empty;
        auto it = prerequisites.find(courseId);
        return (it != prerequisites.end()) ? it->second : empty;
    }
    
    const std::vector<std::string>& getPostrequisites(const std::string& courseId) const {
        static const std::vector<std::string> empty;
        auto it = postrequisites.find(courseId);
        return (it != postrequisites.end()) ? it->second : empty;
    }
    
    bool hasCourse(const std::string& courseId) const {
        return courses.find(courseId) != courses.end();
    }
    
    const Course& getCourse(const std::string& courseId) const {
        return courses.at(courseId);
    }
    
    std::vector<std::string> getAllCourseIds() const {
        std::vector<std::string> ids;
        for (const auto& pair : courses) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    bool hasCycle() const {
        std::unordered_map<std::string, int> visited;
        for (const auto& pair : prerequisites) {
            visited[pair.first] = 0; // 0 = not visited, 1 = in progress, 2 = visited
        }
        
        for (const auto& pair : prerequisites) {
            if (visited[pair.first] == 0) {
                if (hasCycleDFS(pair.first, visited)) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
private:
    bool hasCycleDFS(const std::string& courseId, std::unordered_map<std::string, int>& visited) const {
        visited[courseId] = 1; // In progress
        
        for (const auto& prereq : prerequisites.at(courseId)) {
            if (visited[prereq] == 1) {
                return true; // Cycle detected
            }
            
            if (visited[prereq] == 0) {
                if (hasCycleDFS(prereq, visited)) {
                    return true;
                }
            }
        }
        
        visited[courseId] = 2; // Completed
        return false;
    }
};

//  PathFinder class for finding paths between courses or to satisfy requirements
 
class PathFinder {
private:
    const PrerequisiteGraph& graph;

public:
    PathFinder(const PrerequisiteGraph& pathfinderGraph) : graph(pathfinderGraph) {}
    
    std::vector<std::vector<std::string>> findAllPaths(const std::string& start, const std::string& end) {
        std::vector<std::vector<std::string>> allPaths;
        std::vector<std::string> currentPath;
        std::unordered_set<std::string> visited;
        
        findAllPathsDFS(start, end, visited, currentPath, allPaths);
        return allPaths;
    }
    
    std::vector<std::string> findShortestPath(const std::string& start, const std::string& end) {
        std::unordered_map<std::string, std::string> previous;
        std::queue<std::string> queue;
        std::unordered_set<std::string> visited;
        
        queue.push(start);
        visited.insert(start);
        
        while (!queue.empty()) {
            std::string current = queue.front();
            queue.pop();
            
            if (current == end) {
                // Reconstruct path
                std::vector<std::string> path;
                for (std::string at = end; at != start; at = previous[at]) {
                    path.push_back(at);
                }
                path.push_back(start);
                std::reverse(path.begin(), path.end());
                return path;
            }
            
            for (const auto& next : graph.getPostrequisites(current)) {
                if (visited.find(next) == visited.end()) {
                    visited.insert(next);
                    previous[next] = current;
                    queue.push(next);
                }
            }
        }
        
        return std::vector<std::string>(); // No path found
    }
    
    std::vector<std::string> findRequiredCoursesForTarget(const std::string& target) {
        std::unordered_set<std::string> required;
        std::stack<std::string> stack;
        
        if (!graph.hasCourse(target)) {
        std::cerr << "[ERROR] Target course \"" << target << "\" does not exist.\n";
        return {};  // Prevents bad access
    }

        stack.push(target);
        
        while (!stack.empty()) {
            std::string current = stack.top();
            stack.pop();
            
            if (!graph.hasCourse(current)) {
    std::cerr << "[WARN] Skipping non-existent course: " << current << "\n";
    continue;
}
            for (const auto& prereq : graph.getPrerequisites(current)) {
                if (required.find(prereq) == required.end()) {
                    required.insert(prereq);
                    stack.push(prereq);
                }
            }
        }
        
        return std::vector<std::string>(required.begin(), required.end());
    }
    
private:
    void findAllPathsDFS(const std::string& current, const std::string& end, 
                         std::unordered_set<std::string>& visited,
                         std::vector<std::string>& currentPath,
                         std::vector<std::vector<std::string>>& allPaths) {
        
        visited.insert(current);
        currentPath.push_back(current);
        
        if (current == end) {
            allPaths.push_back(currentPath);
        } else {
            for (const auto& next : graph.getPostrequisites(current)) {
                if (visited.find(next) == visited.end()) {
                    findAllPathsDFS(next, end, visited, currentPath, allPaths);
                }
            }
        }
        
        // Backtrack
        visited.erase(current);
        currentPath.pop_back();
    }
};

// AcademicPlan class for tracking course completion and planning
class AcademicPlan {
private:
    std::string studentId;
    std::string degreePlan;
    std::unordered_map<std::string, bool> completedCourses;
    std::vector<std::vector<std::string>> plannedSemesters;
    std::unordered_map<std::string, float> grades;

public:
    AcademicPlan(const std::string& Id, const std::string& Plan)
        : studentId(Id), degreePlan(Plan) {
        // Initialize with 8 semesters (4 years)
        plannedSemesters.resize(8);
    }
    
    void markCourseCompleted(const std::string& courseId, float grade = 0.0) {
        completedCourses[courseId] = true;
        grades[courseId] = grade;
    }
    
    bool isCourseCompleted(const std::string& courseId) const {
        auto it = completedCourses.find(courseId);
        return it != completedCourses.end() && it->second;
    }
    
    void planCourse(const std::string& courseId, int semester) {
        if (semester >= 0 && static_cast<size_t>(semester) < plannedSemesters.size()) {
            plannedSemesters[static_cast<size_t>(semester)].push_back(courseId);
        }
    }
    
    const std::vector<std::string>& getPlannedCourses(int semester) const {
        if (semester >= 0 && static_cast<size_t>(semester) < plannedSemesters.size()) {
        return plannedSemesters[static_cast<size_t>(semester)];
        } else {
            throw std::out_of_range("Invalid semester index");
        }
    }
    
    std::vector<std::string> getAllCompletedCourses() const {
        std::vector<std::string> courses;
        for (const auto& pair : completedCourses) {
            if (pair.second) {
                courses.push_back(pair.first);
            }
        }
        return courses;
    }
    
    float getGPA() const {
        float totalPoints = 0.0f;
        int totalCourses = 0;
        
        for (const auto& pair : grades) {
            if (completedCourses.at(pair.first)) {
                totalPoints += pair.second;
                totalCourses++;
            }
        }
        
        return (totalCourses > 0) ? (totalPoints / static_cast<float>(totalCourses)) : 0.0f;
    }
    
    void clearPlannedCourse(const std::string& courseId) {
        for (auto& semester : plannedSemesters) {
            auto it = std::find(semester.begin(), semester.end(), courseId);
            if (it != semester.end()) {
                semester.erase(it);
                break;
            }
        }
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Academic Plan for Student: " << studentId << "\n";
        ss << "Degree Plan: " << degreePlan << "\n\n";
        
        ss << "Completed Courses:\n";
        for (const auto& pair : completedCourses) {
            if (pair.second) {
                ss << "  " << pair.first << " (Grade: " << grades.at(pair.first) << ")\n";
            }
        }
        
        ss << "\nPlanned Semesters:\n";
        for (size_t i = 0; i < plannedSemesters.size(); ++i) {
            if (!plannedSemesters[i].empty()) {
                ss << "  Semester " << (i+1) << ": ";
                for (size_t j = 0; j < plannedSemesters[i].size(); ++j) {
                    if (j > 0) ss << ", ";
                    ss << plannedSemesters[i][j];
                }
                ss << "\n";
            }
        }
        
        ss << "\nOverall GPA: " << std::fixed << std::setprecision(2) << getGPA();
        
        return ss.str();
    }
};

 // GraduationValidator class for checking if graduation requirements are met
 
class GraduationValidator {
private:
    const PrerequisiteGraph& graph;
    std::vector<Requirement> degreeRequirements;

public:
    GraduationValidator(const PrerequisiteGraph& prerequisiteGraph) : graph(prerequisiteGraph) {}
    
    void addDegreeRequirement(const Requirement& req) {
        degreeRequirements.push_back(req);
    }
    
    bool validateGraduationPath(const AcademicPlan& plan) const {
        // Check all requirements
        for (const auto& req : degreeRequirements) {
            if (!validateRequirement(req, plan)) {
                return false;
            }
        }
        
        return true;
    }
    
    std::vector<std::string> getMissingRequirements(const AcademicPlan& plan) const {
        std::vector<std::string> missing;
        
        for (const auto& req : degreeRequirements) {
            if (!validateRequirement(req, plan)) {
                missing.push_back(req.getName());
            }
        }
        
        return missing;
    }
    
private:
    bool validateRequirement(const Requirement& req, const AcademicPlan& plan) const {
        switch (req.getType()) {
            case Requirement::COURSE: {
             std::string courseId = req.getCourseOptions()[0];
                return plan.isCourseCompleted(courseId);
            }
            case Requirement::ONE_OF: {
                for (const auto& courseId : req.getCourseOptions()) {
                    if (plan.isCourseCompleted(courseId)) {
                        return true;
                    }
                }
                return false;
            }
            case Requirement::COURSE_CATEGORY: {
                // This would require a more complex implementation with course categories
                // For simplicity, we'll assume it's satisfied
                return true;
            }
            case Requirement::CREDIT_COUNT: {
                // Count credits in completed courses for the given category
                // Again, this is simplified
                return true;
            }
            case Requirement::COMPOUND: {
                for (const auto& subReq : req.getSubRequirements()) {
                    if (!validateRequirement(subReq, plan)) {
                        return false;
                    }
                }
                return true;
            }
            default:
                return false;
        }
    }
};

class CourseDependencyManager {
private:
    std::map<std::string, std::vector<std::string>> prerequisites;
    std::map<std::string, std::vector<std::string>> postrequisites;
    std::map<std::string, Course> courses;

public:
    void addCourse(const Course& course) {
        courses[course.getId()] = course;
        if (prerequisites.find(course.getId()) == prerequisites.end()) {
            prerequisites[course.getId()] = std::vector<std::string>();
        }
        if (postrequisites.find(course.getId()) == postrequisites.end()) {
            postrequisites[course.getId()] = std::vector<std::string>();
        }
    }

    bool addDependency(const std::string& course, const std::string& prereq) {
        if (!hasCourse(course) || !hasCourse(prereq)) return false;
        prerequisites[course].push_back(prereq);
        postrequisites[prereq].push_back(course);
        return true;
    }

    bool hasCourse(const std::string& courseId) const {
        return courses.find(courseId) != courses.end();
    }

    std::vector<std::string> getPrerequisites(const std::string& courseId) const {
        auto it = prerequisites.find(courseId);
        return (it != prerequisites.end()) ? it->second : std::vector<std::string>();
    }
};

// ScheduleAssistant class for helping plan course schedules
 
class ScheduleAssistant {
private:
    const PrerequisiteGraph& graph;

    // Modified to use vector for result and map for tracking
    std::vector<std::string> topologicalSort(const std::vector<std::string>& courses) const {
        if (courses.empty()) {
            return std::vector<std::string>();
        }

        std::map<std::string, bool> visited;
        std::map<std::string, bool> inStack;
        std::vector<std::string> result;

        // Initialize tracking maps
        for (const auto& course : courses) {
            visited[course] = false;
            inStack[course] = false;
        }

        std::function<bool(const std::string&)> dfs = [&](const std::string& course) {
            if (inStack[course]) return false;  // cycle detected
            if (visited[course]) return true;   // already processed

            inStack[course] = true;

            for (const auto& prereq : graph.getPrerequisites(course)) {
                if (std::find(courses.begin(), courses.end(), prereq) != courses.end()) {
                    if (!dfs(prereq)) return false;
                }
            }

            inStack[course] = false;
            visited[course] = true;
            result.push_back(course);
            return true;
        };

        for (const auto& course : courses) {
            if (!visited[course]) {
                if (!dfs(course)) {
                    return std::vector<std::string>(); // Return empty on cycle detection
                }
            }
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

public:
    ScheduleAssistant(const PrerequisiteGraph& prerequisiteGraph) : graph(prerequisiteGraph) {}

   // In the ScheduleAssistant class, modify generateOptimalSchedule:
std::vector<std::vector<std::string>> generateOptimalSchedule(
    const AcademicPlan& plan,
    const std::vector<std::string>& targetCourses,
    int maxSemesters = 8,
    int maxCoursesPerSemester = 5) const {
    
    std::vector<std::vector<std::string>> schedule(static_cast<size_t>(maxSemesters));
    
    try {
        std::vector<std::string> completed(plan.getAllCompletedCourses());
        std::vector<std::string> requiredCourses;
        
        // Collect required courses and their prerequisites
        for (const auto& target : targetCourses) {
            if (!graph.hasCourse(target)) continue;
            if (std::find(requiredCourses.begin(), requiredCourses.end(), target) == requiredCourses.end()) {
                requiredCourses.push_back(target);
            }
            
            PathFinder pathFinder(graph);
            auto prereqs = pathFinder.findRequiredCoursesForTarget(target);
            for (const auto& prereq : prereqs) {
                if (std::find(requiredCourses.begin(), requiredCourses.end(), prereq) == requiredCourses.end()) {
                    requiredCourses.push_back(prereq);
                }
            }
        }
        
        // Remove completed courses
        requiredCourses.erase(
            std::remove_if(requiredCourses.begin(), requiredCourses.end(),
                [&completed](const std::string& course) {
                    return std::find(completed.begin(), completed.end(), course) != completed.end();
                }
            ),
            requiredCourses.end()
        );
        
        auto sortedCourses = topologicalSort(requiredCourses);
        std::vector<std::string> scheduledCourses;
        
        
        // Modified scheduling logic
        for (const auto& course : sortedCourses) {
            bool scheduled = false;
            
            // Try to schedule the course in each semester until successful
            for (size_t semester = 0; semester < static_cast<size_t>(maxSemesters) && !scheduled; ++semester) {
                // Check if prerequisites are met by this semester
                bool prereqsMet = true;
                for (const auto& prereq : graph.getPrerequisites(course)) {
                    if (std::find(completed.begin(), completed.end(), prereq) == completed.end()) {
                        // Check if prerequisite is scheduled in an earlier semester
                        bool prereqScheduled = false;
                        for (size_t s = 0; s < semester; ++s) {
                            if (std::find(schedule[s].begin(), schedule[s].end(), prereq) != schedule[s].end()) {
                                prereqScheduled = true;
                                break;
                            }
                        }
                        if (!prereqScheduled) {
                            prereqsMet = false;
                            break;
                        }
                    }
                }
                
                // Schedule the course if prerequisites are met and semester has space
                if (prereqsMet && schedule[semester].size() < static_cast<size_t>(maxCoursesPerSemester)) {
                    schedule[semester].push_back(course);
                    scheduledCourses.push_back(course);
                    scheduled = true;
                }
            }
        }
        
        return schedule;
        
    } catch (const std::exception& e) {
        std::cerr << "Error generating schedule: " << e.what() << "\n";
        return std::vector<std::vector<std::string>>(static_cast<size_t>(maxSemesters));
    }
}

    std::vector<std::string> getAvailableCourses(const AcademicPlan& plan) const {
        std::vector<std::string> availableCourses;
        std::vector<std::string> completed = plan.getAllCompletedCourses();

        for (const auto& courseId : graph.getAllCourseIds()) {
            if (std::find(completed.begin(), completed.end(), courseId) != completed.end()) {
                continue;
            }

            bool allPrereqsMet = true;
            for (const auto& prereq : graph.getPrerequisites(courseId)) {
                if (std::find(completed.begin(), completed.end(), prereq) == completed.end()) {
                    allPrereqsMet = false;
                    break;
                }
            }

            if (allPrereqsMet) {
                availableCourses.push_back(courseId);
            }
        }
        return availableCourses;
    }
};



// Visualizer class for visualizing prerequisite structure
 
class Visualizer {
private:
    const PrerequisiteGraph& graph;
    
public:
    Visualizer(const PrerequisiteGraph& prerequisiteGraph) : graph(prerequisiteGraph) {}
    
    std::string generateTextGraph() const {
        std::stringstream ss;
        
        for (const auto& courseId : graph.getAllCourseIds()) {
            ss << courseId << " -> ";
            
            const auto& prereqs = graph.getPrerequisites(courseId);
            if (prereqs.empty()) {
                ss << "(No prerequisites)";
            } else {
                for (size_t i = 0; i < prereqs.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << prereqs[i];
                }
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
    std::string generateAsciiTree(const std::string& rootCourse, int maxDepth = 3) const {
        std::stringstream ss;
        std::unordered_set<std::string> visited;
        
        ss << "Prerequisite Tree for " << rootCourse << ":\n";
        generateAsciiTreeHelper(rootCourse, "", true, visited, ss, 0, maxDepth);
        
        return ss.str();
    }
    
private:
    void generateAsciiTreeHelper(
        const std::string& courseId,
        const std::string& prefix,
        bool isLast,
        std::unordered_set<std::string>& visited,
        std::stringstream& ss,
        int depth,
        int maxDepth) const {
        
        if (depth > maxDepth) return;
        
        // Mark as visited to prevent cycles
        visited.insert(courseId);
        
        // Print current course
        ss << prefix;
        ss << (isLast ? "- " : "|- ");
        ss << courseId;
        
        if (graph.hasCourse(courseId)) {
            if (graph.hasCourse(courseId)) {
  if (graph.hasCourse(courseId)) {
    ss << " - " << graph.getCourse(courseId).getName();
} else {
    ss << " (Unknown course)";
}

} else {
    ss << " (Unknown course)";
}

        }
        ss << "\n";
        
        // Get prerequisites
        const auto& prereqs = graph.getPrerequisites(courseId);
        
        // Generate new prefix for children
        std::string newPrefix = prefix + (isLast ? "    " : "│   ");
        
        // Print prerequisites recursively
        for (size_t i = 0; i < prereqs.size(); ++i) {
            // Skip if already visited to avoid cycles
            if (visited.find(prereqs[i]) != visited.end()) continue;
            
            bool isLastChild = (i == prereqs.size() - 1);
            generateAsciiTreeHelper(prereqs[i], newPrefix, isLastChild, visited, ss, depth + 1, maxDepth);
        }
        
        // Remove from visited when backtracking
        visited.erase(courseId);
    }
};

// Demo function
void runDemo() {
   
// Create courses
    std::vector<Course> courses = {
        Course("CS101", "Introduction to Programming", 3, 7),    // Offered all semesters
        Course("CS201", "Data Structures", 3, 7),               
        Course("CS202", "Algorithms", 4, 5),                    // Spring and Fall
        Course("CS301", "Database Systems", 3, 7),
        Course("CS310", "Computer Networks", 3, 5),              // Spring and Fall
        Course("CS320", "Operating Systems", 4, 5),              // Spring and Fall
        Course("CS401", "Software Engineering", 3, 7),
        Course("CS410", "Artificial Intelligence", 3, 5),        // Spring and Fall
        Course("CS420", "Computer Graphics", 3, 5),              // Spring and Fall
        Course("CS430", "Machine Learning", 3, 5),               // Spring and Fall
        Course("CS440", "Parallel Computing", 3, 1),             // Spring only
        Course("CS450", "Computer Security", 3, 1),              // Spring only
        Course("MATH101", "Calculus I", 4, 7),
        Course("MATH201", "Calculus II", 4, 7),
        Course("MATH202", "Linear Algebra", 3, 7),
        Course("MATH301", "Discrete Mathematics", 3, 5),         // Spring and Fall
        Course("PHYS101", "Physics I", 4, 7),
        Course("PHYS201", "Physics II", 4, 7)
    };
    
    // Create the prerequisite graph
    PrerequisiteGraph graph;

    
    
    // Add courses to the graph
    for (const auto& course : courses) {
        graph.addCourse(course);
    }
    
    // Define prerequisites
    graph.addPrerequisite("CS201", "CS101");
    graph.addPrerequisite("CS202", "CS201");
    graph.addPrerequisite("CS301", "CS201");
    graph.addPrerequisite("CS310", "CS201");
    graph.addPrerequisite("CS320", "CS201");
    graph.addPrerequisite("CS320", "MATH301");
    graph.addPrerequisite("CS401", "CS301");
    graph.addPrerequisite("CS401", "CS320");
    graph.addPrerequisite("CS410", "CS202");
    graph.addPrerequisite("CS410", "MATH202");
    graph.addPrerequisite("CS420", "CS202");
    graph.addPrerequisite("CS420", "MATH202");
    graph.addPrerequisite("CS430", "CS410");
    graph.addPrerequisite("CS430", "MATH202");
    graph.addPrerequisite("CS440", "CS320");
    graph.addPrerequisite("CS450", "CS310");
    graph.addPrerequisite("MATH201", "MATH101");
    graph.addPrerequisite("MATH202", "MATH101");
    graph.addPrerequisite("MATH301", "MATH101");
    graph.addPrerequisite("PHYS201", "PHYS101");
    graph.addPrerequisite("PHYS201", "MATH101");
    
    // Create degree requirements
    std::vector<Requirement> csDegreeRequirements;
    
    // Core CS requirements
    Requirement csCore("CS Core", Requirement::COMPOUND);
    
    Requirement cs101Req("CS101 Requirement", Requirement::COURSE);
    cs101Req.addCourseOption("CS101");
    
    Requirement cs201Req("CS201 Requirement", Requirement::COURSE);
    cs201Req.addCourseOption("CS201");
    
    Requirement cs202Req("CS202 Requirement", Requirement::COURSE);
    cs202Req.addCourseOption("CS202");
    
    Requirement cs301Req("CS301 Requirement", Requirement::COURSE);
    cs301Req.addCourseOption("CS301");
    
    Requirement cs320Req("CS320 Requirement", Requirement::COURSE);
    cs320Req.addCourseOption("CS320");
    
    Requirement cs401Req("CS401 Requirement", Requirement::COURSE);
    cs401Req.addCourseOption("CS401");
    
    csCore.addSubRequirement(cs101Req);
    csCore.addSubRequirement(cs201Req);
    csCore.addSubRequirement(cs202Req);
    csCore.addSubRequirement(cs301Req);
    csCore.addSubRequirement(cs320Req);
    csCore.addSubRequirement(cs401Req);
    
    // Math requirements
    Requirement mathReqs("Math Requirements", Requirement::COMPOUND);
    
    Requirement math101Req("MATH101 Requirement", Requirement::COURSE);
    math101Req.addCourseOption("MATH101");
    
    Requirement math201Req("MATH201 Requirement", Requirement::COURSE);
    math201Req.addCourseOption("MATH201");
    
    Requirement math202Req("MATH202 Requirement", Requirement::COURSE);
    math202Req.addCourseOption("MATH202");
    
    Requirement math301Req("MATH301 Requirement", Requirement::COURSE);
    math301Req.addCourseOption("MATH301");
    
    mathReqs.addSubRequirement(math101Req);
    mathReqs.addSubRequirement(math201Req);
    mathReqs.addSubRequirement(math202Req);
    mathReqs.addSubRequirement(math301Req);
    
    // CS Electives requirement
    Requirement csElectives("CS Electives", Requirement::ONE_OF);
    csElectives.addCourseOption("CS410");
    csElectives.addCourseOption("CS420");
    csElectives.addCourseOption("CS430");
    csElectives.addCourseOption("CS440");
    csElectives.addCourseOption("CS450");
    
    // Physics requirements
    Requirement physReqs("Physics Requirements", Requirement::COMPOUND);
    
    Requirement phys101Req("PHYS101 Requirement", Requirement::COURSE);
    phys101Req.addCourseOption("PHYS101");
    
    Requirement phys201Req("PHYS201 Requirement", Requirement::COURSE);
    phys201Req.addCourseOption("PHYS201");
    
    physReqs.addSubRequirement(phys101Req);
    physReqs.addSubRequirement(phys201Req);
    
    // Add all requirements to the degree
    csDegreeRequirements.push_back(csCore);
    csDegreeRequirements.push_back(mathReqs);
    csDegreeRequirements.push_back(csElectives);
    csDegreeRequirements.push_back(physReqs);
    
    // Create a graduation validator
    GraduationValidator validator(graph);
    for (const auto& req : csDegreeRequirements) {
        validator.addDegreeRequirement(req);
    }
    
    // Create a sample academic plan
    AcademicPlan samplePlan("S12005", "Computer Science BS");
    
    // Mark some courses as completed
    samplePlan.markCourseCompleted("CS101", 4.0f);
    samplePlan.markCourseCompleted("CS201", 3.7f);
    samplePlan.markCourseCompleted("MATH101", 3.3f);
    samplePlan.markCourseCompleted("MATH201", 3.0f);
    samplePlan.markCourseCompleted("PHYS101", 3.5f);
    samplePlan.markCourseCompleted("MATH301", 3.5f);  // For CS320->CS401 chain

    // Plan remaining courses
    
    samplePlan.planCourse("CS310", 3); // 4th semester
    
    
    samplePlan.planCourse("CS401", 4); // 5th semester
    samplePlan.planCourse("CS410", 4);
    samplePlan.planCourse("PHYS201", 4);
    
    // Plan courses for earlier and later semesters
samplePlan.planCourse("CS440", 5);   // Semester 6
samplePlan.planCourse("CS450", 6);   // Semester 7
samplePlan.planCourse("CS430", 7);   // Semester 8
    // Create path finder
    PathFinder pathFinder(graph);
    
    // Create schedule assistant
    ScheduleAssistant scheduleAssistant(graph);
    
    // Create visualizer
    Visualizer visualizer(graph);
    
    // Main menu loop
    bool running = true;
    while (running) {
        std::cout << "\n/======================================/\n";
        std::cout << "   COURSE PREREQUISITE CHECKER\n";
        std::cout << "/======================================/\n";
        std::cout << "1. View all courses\n";
        std::cout << "2. View prerequisite structure\n";
        std::cout << "3. View academic plan\n";
        std::cout << "4. Check graduation path\n";
        std::cout << "5. Find path between courses\n";
        std::cout << "6. Generate optimal schedule\n";
        std::cout << "7. View available courses\n";
        std::cout << "8. Visualize prerequisite tree\n";
        std::cout << "9. Add a new course \n";
        std::cout << "10. Add a prerequisite \n";
        std::cout << "11. Exit\n";
        std::cout << "Enter choice: ";
        
        int choice;
        std::cin >> choice;
        
        std::string courseId, prereqId;
        std::vector<std::string> missingReqs;
        
        switch (choice) {
            case 1: // View all courses
                std::cout << "\nAll Available Courses:\n";
                std::cout << "------------------------\n";
                for (const auto& course : courses) {
                    std::cout << course.toString() << "\n";
                    std::cout << "  Offered in: " << course.getSemestersString() << "\n";
                    std::cout << "  Prerequisites: ";
                    const auto& prereqs = graph.getPrerequisites(course.getId());
                    if (prereqs.empty()) {
                        std::cout << "None\n";
                    } else {
                        for (size_t i = 0; i < prereqs.size(); ++i) {
                            if (i > 0) std::cout << ", ";
                            std::cout << prereqs[i];
                        }
                        std::cout << "\n";
                    }
                    std::cout << "\n";
                }
                break;
                
            case 2: // View prerequisite structure
                std::cout << "\nPrerequisite Structure:\n";
                std::cout << visualizer.generateTextGraph();
                break;
                
            case 3: // View academic plan
                std::cout << "\nCurrent Academic Plan:\n";
                std::cout << samplePlan.toString() << "\n";
                break;
                
            case 4: // Check graduation path
                std::cout << "\nChecking Graduation Path...\n";
                if (validator.validateGraduationPath(samplePlan)) {
                    std::cout << "All graduation requirements are satisfied!\n";
                } else {
                    missingReqs = validator.getMissingRequirements(samplePlan);
                    std::cout << "Missing requirements:\n";
                    for (const auto& req : missingReqs) {
                        std::cout << "- " << req << "\n";
                    }
                }
                break;
                
            case 5: // Find path between courses
                std::cout << "Enter source course ID: ";
                std::cin >> courseId;
                std::cout << "Enter target course ID: ";
                std::cin >> prereqId;
                
                if (!graph.hasCourse(courseId) || !graph.hasCourse(prereqId)) {
                    std::cout << "Invalid course ID(s).\n";
                } else {
                    std::cout << "\nFinding paths from " << courseId << " to " << prereqId << "...\n";
                    
                    std::vector<std::string> shortestPath = pathFinder.findShortestPath(courseId, prereqId);
                    
                    if (shortestPath.empty()) {
                        std::cout << "No path found.\n";
                    } else {
                        std::cout << "Shortest path:\n";
                        for (size_t i = 0; i < shortestPath.size(); ++i) {
                            if (i > 0) std::cout << " -> ";
                            std::cout << shortestPath[i];
                        }
                        std::cout << "\n";
                    }
                    
                    std::cout << "\nAll possible paths:\n";
                    std::vector<std::vector<std::string>> allPaths = pathFinder.findAllPaths(courseId, prereqId);
                    
                    if (allPaths.empty()) {
                        std::cout << "No paths found.\n";
                    } else {
                        for (size_t i = 0; i < allPaths.size(); ++i) {
                            std::cout << "Path " << (i+1) << ": ";
                            for (size_t j = 0; j < allPaths[i].size(); ++j) {
                                if (j > 0) std::cout << " -> ";
                                std::cout << allPaths[i][j];
                            }
                            std::cout << "\n";
                        }
                    }
                }
                break;
                
          case 6: // Generate optimal schedule
{
    std::cout << "\nGenerating Optimal Schedule\n";
    std::cout << "----------------------------\n";

    // Get both completed and planned courses
    std::vector<std::string> completed = samplePlan.getAllCompletedCourses();
    std::cout << "Already completed courses: ";
    for (const auto& course : completed) {
        std::cout << course << " ";
    }
    std::cout << "\n\n";

    // Define target courses that need to be taken
    std::vector<std::string> targets;
    // Add courses that aren't completed yet
    if (std::find(completed.begin(), completed.end(), "CS202") == completed.end()) 
        targets.push_back("CS202");
    if (std::find(completed.begin(), completed.end(), "CS301") == completed.end()) 
        targets.push_back("CS301");
    if (std::find(completed.begin(), completed.end(), "CS320") == completed.end()) 
        targets.push_back("CS320");
    if (std::find(completed.begin(), completed.end(), "CS401") == completed.end()) 
        targets.push_back("CS401");
    if (std::find(completed.begin(), completed.end(), "MATH202") == completed.end()) 
        targets.push_back("MATH202");
    if (std::find(completed.begin(), completed.end(), "PHYS201") == completed.end()) 
        targets.push_back("PHYS201");

    try {
        auto schedule = scheduleAssistant.generateOptimalSchedule(samplePlan, targets);
        
        // Display the schedule with prerequisites check
        bool hasScheduledCourses = false;
        for (size_t i = 0; i < schedule.size(); ++i) {
            if (!schedule[i].empty()) {
                hasScheduledCourses = true;
                std::cout << "Semester " << (i+1) << ": ";
                for (size_t j = 0; j < schedule[i].size(); ++j) {
                    if (j > 0) std::cout << ", ";
                    std::cout << schedule[i][j];
                    
                    // Show prerequisites for each course
                    auto prereqs = graph.getPrerequisites(schedule[i][j]);
                    if (!prereqs.empty()) {
                        std::cout << " (requires: ";
                        for (size_t k = 0; k < prereqs.size(); ++k) {
                            if (k > 0) std::cout << ", ";
                            std::cout << prereqs[k];
                        }
                        std::cout << ")";
                    }
                }
                std::cout << "\n";
            }
        }

        if (!hasScheduledCourses) {
            std::cout << "No courses to schedule (all prerequisites may be completed).\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error generating schedule: " << e.what() << "\n";
    }
}
break;
                
         case 7: // View available courses
    std::cout << "\nAvailable Courses (prerequisites satisfied):\n";
    try {
        auto availableCourses = scheduleAssistant.getAvailableCourses(samplePlan);

        if (availableCourses.empty()) {
            std::cout << "No available courses at this time.\n";
        } else {
            for (const auto& availableCourseId : availableCourses) {
                if (graph.hasCourse(availableCourseId)) {
                    std::cout << "- " << graph.getCourse(availableCourseId).toString() << "\n";
                } else {
                    std::cout << "- " << availableCourseId << " (Course not found in graph)\n";
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error while fetching available courses: " << e.what() << "\n";
    }
    break;
                
            case 8: // Vi sualize prerequisite tree
                std::cout << "Enter course ID to visualize: ";
                std::cin >> courseId;
                
                if (!graph.hasCourse(courseId)) {
                    std::cout << "Invalid course ID.\n";
                } else {
                    std::cout << "\n" << visualizer.generateAsciiTree(courseId) << "\n";
                }
                break;
                
            case 9: // Add a new course
    {
        std::string newCourseId, courseName;
        int credits, semesterOffered;

        std::cout << "Enter course ID: ";
        std::cin >> newCourseId;
        std::cout << "Enter course name: ";
        std::cin.ignore(); // Ignore leftover newline
        std::getline(std::cin, courseName);
        std::cout << "Enter credits: ";
        std::cin >> credits;
        std::cout << "Enter semester offered (1=Spring, 2=Summer, 4=Fall, combine with +): ";
        std::cin >> semesterOffered;

        Course newCourse(newCourseId, courseName, credits, semesterOffered);
        graph.addCourse(newCourse);
        std::cout << "Course added successfully!\n";
    }
    break;

        case 10: // Add a prerequisite
    {
        std::string maincourseId, prerequisiteId;

        std::cout << "Enter course ID: ";
        std::cin >> maincourseId;
        std::cout << "Enter prerequisite course ID: ";
        std::cin >> prerequisiteId;

        if (!graph.hasCourse(maincourseId) || !graph.hasCourse(prerequisiteId)) {
            std::cout << "Invalid course ID(s). Make sure both courses exist.\n";
        } else {
            graph.addPrerequisite(maincourseId, prerequisiteId);
            std::cout << "Prerequisite added successfully!\n";
        }
    }
    break;
            case 11: // Exit
                running = false;
                
                break;
                
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {
    runDemo();
    return 0;
}