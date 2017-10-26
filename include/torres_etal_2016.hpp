/**
* @file torres_etal_2016.hpp
* @brief Header file for torres_etal_2016.cpp
* @author Takaki Ueno
*/

#ifndef INCLUDED_torres_etal_2016_hpp_
#define INCLUDED_torres_etal_2016_hpp_

// roscpp
#include <ros/ros.h>

// cpp standard libraries
#include <array>
#include <vector>
#include <cmath>

// std_msgs
#include <std_msgs/Float64.h>

// geometry_msgs
#include <geometry_msgs/Point.h>

// Boost
#include <boost/range/adaptor/indexed.hpp>

// cgutil
#include <cgutil.hpp>

/**
* @struct Direction
* @brief Storage for line sweep direction
* 
* Sweep direction is from edge to vertex
*/
struct Direction
{
    std::array<geometry_msgs::Point, 2> base_edge;
    geometry_msgs::Point opposed_vertex;
};

/**
* @brief Calculates line sweep direction for given polygon
* @param polygon Line sweep direction is calculated on this region
* @return direction Struct containing edge and vertex
*/
Direction calc_sweep_direction(std::vector<geometry_msgs::Point> &polygon)
{
    Direction line_sweep;

    // Edges of polygon
    std::vector<std::array<geometry_msgs::Point, 2>> edges;

    // Make a list of edges of polygon
    for (std::size_t i = 0; i < polygon.size(); ++i)
    {
        std::array<geometry_msgs::Point, 2> ar;

        ar.at(0) = polygon.at(i);

        // if vertex is the last one,
        // that vertex makes an edge whose end is the first vertex
        if (i == polygon.size() - 1)
        {
            ar.at(1) = polygon.at(0);
        }
        else
        {
            ar.at(1) = polygon.at(i + 1);
        }
        edges.push_back(ar);
    }

    double optimal_dist = 0;

    // Calculate line sweep direction
    // Algorithm 1 in Torres et al, 2016
    for (const auto &edge : edges | boost::adaptors::indexed())
    {
        double max_dist_edge = 0;
        geometry_msgs::Point opposed_vertex;

        ROS_DEBUG("e1.x: %f", edge.value().front().x);
        ROS_DEBUG("e1.y: %f", edge.value().front().y);
        ROS_DEBUG("e2.x: %f", edge.value().back().x);
        ROS_DEBUG("e2.y: %f", edge.value().back().y);

        for (const geometry_msgs::Point &vertex : polygon)
        {
            // calc_distance() function returns distance
            // between given edge and vertex
            double dist = calc_distance(edge.value(), vertex);

            ROS_DEBUG("vertex.x: %f", vertex.x);
            ROS_DEBUG("vertex.y: %f", vertex.y);
            ROS_DEBUG("distance: %f", dist);

            if (dist > max_dist_edge)
            {
                max_dist_edge = dist;
                opposed_vertex = vertex;
                ROS_DEBUG("max_dist_edge: %f", max_dist_edge);
            }
        }

        if ((max_dist_edge < optimal_dist) or edge.index() == 0)
        {
            optimal_dist = max_dist_edge;
            line_sweep.base_edge = edge.value();
            line_sweep.opposed_vertex = opposed_vertex;
            ROS_DEBUG("index: %ld", edge.index());
            ROS_DEBUG("optimal dist: %f", optimal_dist);
        }
    }

    ROS_DEBUG("End");

    return line_sweep;
}

#endif