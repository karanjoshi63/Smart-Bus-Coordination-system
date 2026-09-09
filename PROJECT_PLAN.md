# 📋 Project Plan — Smart Bus Coordination System

This document outlines the complete plan for our project — from the problem 
we identified, to our solution approach, to how we're building it step by 
step, and where we see this project going in the future.

---

## 1. Problem Statement

Our university's bus service operates on only three fixed departure times — 
**1:00 PM, 4:00 PM, and 6:00 PM**. However, class schedules vary significantly 
across sections, with different sections ending their classes at different 
times throughout the day.

This mismatch causes a major issue: a student whose class ends at, say, 
2:00 PM has to wait until 4:00 PM for the next bus, wasting nearly 2 hours 
doing nothing productive.

This problem is especially significant for students who commute long 
distances and rely entirely on the university bus service. Waiting 2-3 hours 
after class also raises safety concerns, particularly during evening hours, 
and discourages students from staying back for extra academic work.

## 2. Current Situation (State of the Art)

There is currently no software-based system at our university that connects 
class timetables with bus schedules. Students rely on informal methods — 
asking seniors, checking group chats, or simply memorizing the fixed 
schedule — to plan around bus timings. The academic timetable and the bus 
schedule are managed completely independently, with no communication 
between the two.

## 3. Our Solution — Explained in Levels

We are solving this problem in three progressive levels, each building on 
the previous one:

### Level 1 — Basic Matching
Given any class's end time, the system instantly finds the next available 
fixed bus (1 PM / 4 PM / 6 PM) and generates a report showing which sections 
face the most waiting time under the current fixed schedule.

### Level 2 — Demand Detection
If a large number of students' classes end around the same time, the system 
flags this as a high-demand slot where an additional bus could be justified, 
instead of forcing everyone to wait for the next fixed slot.

### Level 3 — On-Demand Request System
Students can submit a request after their class ends, specifying their 
class end-time and nearest bus stop (chosen from a predefined list). The 
system groups requests by stop, and once a minimum threshold of students is 
reached at a stop, an on-demand bus is assigned. If the threshold isn't met 
in time, those students automatically fall back to the nearest fixed bus.

## 4. Step-by-Step Build Plan

We are building this project incrementally, testing each part before moving 
to the next:

**Step 1 — Class Skeletons**
Define the basic structure of our core classes (`Section`, `Bus`, `Student`, 
`Request`, `Stop`) with just their data members and constructors — no logic 
yet. This gives the whole team a shared foundation to build on.

**Step 2 — Core Matching Logic**
Implement a `vector` of fixed bus timings, sort it, and write a binary 
search function that takes a class end-time and returns the next available 
bus. This is the simplest, most essential piece and everything else depends 
on it.

**Step 3 — Request & Queue System**
Build the request-handling flow: students submit a request, it gets added to 
a `queue` for their chosen stop, and requests are grouped using a `HashMap` 
(stop name → queue of requests).

**Step 4 — Threshold & Fallback Logic**
Write the logic that checks whether a stop has reached its threshold number 
of waiting students. If yes, an on-demand bus is suggested; if not, students 
fall back to the nearest fixed bus (using the Step 2 function).

**Step 5 — Route Network (Graph)**
Represent the network of bus stops and routes as a `graph`, with travel time 
as edge weights. Implement Dijkstra's algorithm to find the most time-
efficient route from any stop to the university, especially useful when 
multiple buses cover different, overlapping routes.

**Step 6 — OOP Concepts**
Apply Inheritance (e.g., a `Student` base class extended by classes that 
need additional request-specific data) and Polymorphism (e.g., a `Bus` base 
class with a `showInfo()` method overridden differently by fixed vs. 
on-demand bus types), alongside Encapsulation and Abstraction already used 
throughout the codebase.

**Step 7 — Undo & Menu Interface**
Add a `stack`-based undo history (per student, so one student's undo never 
affects another's) and a console menu (`main.cpp`) that ties all the above 
features together into a usable application.

**Step 8 — Integration & Testing**
Combine all modules, test the complete flow end-to-end (request → threshold 
check → bus assignment → undo), and fix bugs found during testing.

## 5. Tech Stack

- **Language:** C++ (Standard: C++17)
- **Paradigm:** Object-Oriented Programming
- **Data Structures used:** Vector, Sorting, Binary Search, HashMap 
  (`unordered_map`), Queue, Priority Queue, Graph (adjacency list), Stack
- **OOP Concepts used:** Encapsulation, Abstraction, Inheritance, 
  Polymorphism (Function Overloading & Overriding)
- **Tools:** g++ compiler, VS Code / Code::Blocks, Git & GitHub for version 
  control

## 6. Team & Roles

| Role | Responsibilities |
|---|---|
| Core Logic Lead | Bus timings (vector + sorting), binary search matching, wait-time report |
| OOP Lead | Designing and implementing core classes (`Section`, `Bus`, `Student`, etc.) |
| Request & Threshold Lead | Request handling, queue-based grouping, threshold logic, fallback |
| Route & Graph Lead | Graph-based route network, Dijkstra's algorithm for route efficiency |
| Interface & Testing Lead | Menu system, stack-based undo, integration, testing, documentation |

## 7. Future Scope

This project currently runs as a console-based C++ application, since our 
primary goal is to demonstrate strong fundamentals in Object-Oriented 
Programming and Data Structures for our college evaluation.

Going forward, we plan to extend this into a **full-fledged web or mobile 
application**, where:

- Students can submit bus requests directly from their phone or browser, 
  instead of a terminal interface.
- The system can show live/real-time demand at each stop, visible to both 
  students and university transport staff.
- The backend logic (matching, threshold detection, route optimization) 
  built in this project would serve as the core engine, with a REST API 
  layer connecting it to a proper frontend (built with React or similar).
- Real GPS-based stop data and live traffic conditions could replace the 
  current fixed-distance assumptions used in the route graph.

This console application is intentionally the first step — a solid, well-
structured foundation that proves the core logic works, before investing 
time into a full user-facing product.

## 8. Assumptions

- The list of bus stops is fixed and predefined, not based on live GPS 
  coordinates.
- The demand threshold (minimum students needed for an on-demand bus) is a 
  fixed value decided based on average bus seating capacity, not dynamically 
  calculated.
- Travel times between stops are fixed estimated values, not based on 
  real-time traffic data.
