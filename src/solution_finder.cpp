#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int8_multi_array.hpp"
#include "std_msgs/msg/int8.hpp"

class SolutionFinder : public rclcpp::Node
{
public:
    SolutionFinder()
        : Node("solution_finder"), input_received_(false), target_received_(false) //initializing the solution_finder node
    {
        //create a subscription to /input topic that listens for "Int8MultiArray" messages
        input_subscriber_ = this->create_subscription<std_msgs::msg::Int8MultiArray>(
            "/input", 10, std::bind(&SolutionFinder::input_callback, this, std::placeholders::_1));
       
        //create a subscription to /input topic that listens for "Int8" messages
        target_subscriber_ = this->create_subscription<std_msgs::msg::Int8>(
            "/target", 10, std::bind(&SolutionFinder::target_callback, this, std::placeholders::_1));

        //create a publisher to /solution topic to send the results of type Int8MultiArray
        solution_publisher_ = this->create_publisher<std_msgs::msg::Int8MultiArray>("/solution", 10);

        //create a timer to publish at 1 Hz
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1), std::bind(&SolutionFinder::find_and_publish_solution, this)); // 1 Hz rate
    }

private:
    void input_callback(const std_msgs::msg::Int8MultiArray::SharedPtr msg)
    {
        input_ = msg->data; //storing input data 
        input_received_ = true; // flag to indicate input has been received
    }

    void target_callback(const std_msgs::msg::Int8::SharedPtr msg)
    {
        target_ = msg->data; //store target data 
        target_received_ = true; // flag to indicate target has been received
    }

    void find_and_publish_solution()
    {
        if (!input_received_ || !target_received_) //proceed if both inputs are found else return
        {
            return;
        }

        for (size_t i = 0; i < input_.size(); ++i)
        {
            for (size_t j = i + 1; j < input_.size(); ++j)
            {
                if (input_[i] + input_[j] == target_.value()) //check if target made with i and j values
                {
                    //create mssg to publish the data 
                    std_msgs::msg::Int8MultiArray solution_msg; 
                    solution_msg.data = {static_cast<int8_t>(i), static_cast<int8_t>(j)};

                    //publish solution to /solution topic using defined solution publisher 
                    solution_publisher_->publish(solution_msg);
                    return;
                }
            }
        }
    }

    //subscribers for input array and target value  
    rclcpp::Subscription<std_msgs::msg::Int8MultiArray>::SharedPtr input_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr target_subscriber_;

    //publisher to send the solution
    rclcpp::Publisher<std_msgs::msg::Int8MultiArray>::SharedPtr solution_publisher_;
    
    //variables to store input array and target 
    std::vector<int8_t> input_;
    std::optional<int8_t> target_;
    
    //flags to indicate if input and target are received
    bool input_received_;
    bool target_received_;

    //timer for 1 Hz publishing rate
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv); //initialize ros2
    rclcpp::spin(std::make_shared<SolutionFinder>()); //create instance of SolutionFinder node and start spinning it to process callbacks
    rclcpp::shutdown();
    return 0;
}

