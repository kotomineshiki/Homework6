#include <iostream>  // basic input output
#include <memory>    // for make_shared

#include <homework6.hpp>      // header file
#include <opengl_helper.hpp>  // helper library
// glm library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
const unsigned int MyScreenWidth = 800;
const unsigned int MyScreenHeight = 600;

auto camera = std::make_shared<helper::Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
// make mouse initial position center
float last_x = MyScreenWidth / 2.0f;
float last_y = MyScreenHeight / 2.0f;
bool first_move_mouse = true;
// timing
// time between current frame and last frame
float delta_time = 0.0f;
float last_frame = 0.0f;
// ImGui
bool can_mouse_move_camera = false;
bool use_gouraud_model = false;
float light_strength[4] {0.1f, 1.0f, 0.5f, 1.0f};
glm::vec3 light_position(0.4f, -0.5f, 0.8f);
float objectColor[3] = { 0.0f,1.0f,0.0f };

int main() {
  // the GLFW window
  GLFWwindow* window;
  // save the vao vbo eao
  GLuint box[3], light;
  // store window size
  int width = MyScreenWidth, height = MyScreenHeight;

  // basic v
  std::vector<GLfloat> vertex{
      -0.5f, -0.5f, -0.5f,      0.5f,  -0.5f, -0.5f,
      0.5f,  0.5f,  -0.5f, //三角形1
                                            0.5f,  0.5f,  -0.5f, 
      -0.5f, 0.5f,  -0.5f,     -0.5f, -0.5f, -0.5f, //面1



      -0.5f, -0.5f, 0.5f,       0.5f,  -0.5f, 0.5f,  
      0.5f,  0.5f,  0.5f,       
                                           0.5f,  0.5f,  0.5f,  
      -0.5f, 0.5f,  0.5f,       -0.5f, -0.5f, 0.5f,  //面2



      -0.5f, 0.5f,  0.5f,       -0.5f, 0.5f,  -0.5f, 
      -0.5f, -0.5f, -0.5f,      
                                           -0.5f, -0.5f, -0.5f, 
      -0.5f, -0.5f, 0.5f,     -0.5f, 0.5f,  0.5f, //面3



      0.5f,  0.5f,  0.5f,     0.5f,  0.5f,  -0.5f, 
      0.5f,  -0.5f, -0.5f, 
                                           0.5f,  -0.5f, -0.5f,
      0.5f,  -0.5f, 0.5f,      0.5f,  0.5f,  0.5f, //面4



      -0.5f, -0.5f, -0.5f,   0.5f,  -0.5f, -0.5f, 
      0.5f,  -0.5f, 0.5f,       
                                           0.5f,  -0.5f, 0.5f,  
      -0.5f, -0.5f, 0.5f,      -0.5f, -0.5f, -0.5f, //面5

      -0.5f, 0.5f,  -0.5f,      0.5f,  0.5f,  -0.5f, 
      0.5f,  0.5f,  0.5f,       
                                           0.5f,  0.5f,  0.5f,
      -0.5f, 0.5f,  0.5f,     -0.5f, 0.5f,  -0.5f};//面6

  auto add_normal_vector = [](std::vector<GLfloat>& vertex, int n) {
    glm::vec3 sides[3] = {glm::vec3()};
    for (int i = 0; 3 * (n + 3) * i < (int)vertex.size(); i++) {
      auto index = (i * (3 * (n + 3)));
      auto p = vertex.data() + index;
      for (int j = 0; j < 3; j++) {
        sides[j] = glm::vec3(p[n * j + 0], p[n * j + 1], p[n * j + 2]);
      }
      auto normal =
          glm::normalize(glm::cross(sides[2] - sides[1], sides[0] - sides[1]));
      std::vector<GLfloat> normal_v{normal[0], normal[1], normal[2]};//法向量
      for (int j = 0; j < 3; j++) {
        vertex.insert(vertex.begin() + (index + (n + 3) * (j + 1) - 3), normal_v.begin(),
                 normal_v.end());
      }
    }
  };
  // dirty work initial
  auto initial_window = [&window, width, height] {//捕获外部有用变量
    window = glfwCreateWindow(width, height, "homework6", NULL, NULL);
    helper::assert_true(window != NULL, "Failed to create GLFW windows");
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
  };
  // if we need update vao's vbo / eao
  auto update_box = []() {};
  // initial vao
  auto set_box = [update_box, &vertex](GLuint VAO, GLuint VBO, GLuint EBO) {
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(),
                 vertex.data(), GL_STATIC_DRAW);
    //将数据拷贝到缓存对象
    //GL_ARRAY_BUFFER||GL_ELEMENT_ARRAY
    //传递数据的字节数
    //为源数组提供执政
    //提供缓存对象的使用方法
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (void*)0);
    //指定渲染时索引值为index的顶点属性数组的数据格式和位置（
    //要修改的顶点的索引值，每个顶点属性的组件数量，参数个数
    //组件的数据类型
    //是否被归一化
    //周期性（多少单位一个周期）
    //第一个组件在数组的顶点属性中的偏移量）
    glEnableVertexAttribArray(0);
    //顶点位置属性

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));
    //5开始的三个
    glEnableVertexAttribArray(1);
    update_box();
  };

  auto set_light = [&vertex](GLuint VAO, GLuint VBO, GLuint EBO) {
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(),
                 vertex.data(), GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (void*)0);

    glEnableVertexAttribArray(0);
    // texture coord attribute

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
  };

  
  auto create_imgui = []() {//创建IMGUI窗口
    ImGui::Begin("Menu");
    ImGui::Text("Welcome");
    ImGui::Checkbox("use_gouraud_model", &use_gouraud_model);
    ImGui::SliderFloat("light x", &light_position[0], -1, 1);
    ImGui::SliderFloat("light y", &light_position[1], -1, 1);
    ImGui::SliderFloat("light z", &light_position[2], -1, 1);
    ImGui::SliderFloat("ambientStrength", light_strength, 0, 1);
    ImGui::SliderFloat("diffuseStrength", light_strength + 1, 0, 1);
    ImGui::SliderFloat("specularStrength", light_strength + 2, 0, 1);
    ImGui::SliderFloat("illuminance", light_strength + 3, 0, 1);
    ImGui::ColorEdit3("Color",objectColor,0);
    ImGui::End();
  };

  auto update_delta = [ ]() {//得到新的时间差
    float currentFrame = (float)glfwGetTime();
    delta_time = currentFrame - last_frame;
    last_frame = currentFrame;
  };

  helper::InitialOpenGL(initial_window, window);//初始化OpenGL窗口
  glEnable(GL_DEPTH_TEST);//开启深度测试

  // create shader program
  GLuint phong_shader_program = helper::CreatProgramWithShader(
      "E:\\GitHub\\Computer-Graphics\\6 - Lights and Shading\\resources\\shaders\\phong.vs",
      "E:\\GitHub\\Computer-Graphics\\6 - Lights and Shading\\resources\\shaders\\phong.fs");
  GLuint light_shader_program =
      helper::CreatProgramWithShader("E:\\GitHub\\Computer-Graphics\\6 - Lights and Shading\\resources\\shaders\\light.vs",
                                     "E:\\GitHub\\Computer-Graphics\\6 - Lights and Shading\\resources\\shaders\\light.fs");
  GLuint gouraud_shader_program =
      helper::CreatProgramWithShader("E:\\GitHub\\Computer-Graphics\\6 - Lights and Shading\\resources\\shaders\\gouraud.vs",
                                     "E:\\GitHub\\Computer-Graphics\\6 - Lights and Shading\\resources\\shaders\\gouraud.fs");


  add_normal_vector(vertex, 3);
  for (int i = 0; i < vertex.size(); ++i) {
    std::cout << vertex[i] << ' ';
    if (i % 6 == 5)cout << endl;
  }

  helper::SetVAO(box[0], box[1], box[2], set_box);
  helper::SetVAO(light, box[1], box[2], set_light, false);

  while (!glfwWindowShouldClose(window)) {//渲染循环
    glfwPollEvents();
    update_delta();
    ProcessInput(window);
    glfwGetWindowSize(window, &width, &height);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplGlfwGL3_NewFrame();
    create_imgui();


    //变换矩阵初始化
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    GLuint shader_program=-1;
    if (use_gouraud_model == true) {
      shader_program = gouraud_shader_program;
    }
    else {
      shader_program = phong_shader_program;
    }
    glUseProgram(shader_program);



    model = glm::mat4(1.0f);
    model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    projection = glm::perspective(glm::radians(camera->Zoom),
                                  (float)MyScreenWidth / (float)MyScreenHeight,
                                  0.1f, 100.0f);

    view = camera->GetViewMatrix();//获得像机
    // 向shader传递所需要的数据
    auto SendDataToShader = [&shader_program,&projection,&view,&model](){
      helper::SetShaderMat4(shader_program, "projection", projection);
      helper::SetShaderMat4(shader_program, "view", view);
      helper::SetShaderMat4(shader_program, "model", model);

      helper::SetShaderVec3(shader_program, "objectColor",objectColor[0],objectColor[1],objectColor[2]);
      helper::SetShaderVec3(shader_program, "lightColor", 1.0f, 1.0f, 1.0f);
      helper::SetShaderVec3(shader_program, "lightPos", light_position);
      helper::SetShaderVec3(shader_program, "viewPos", camera->Position);
      helper::SetShaderFloat(shader_program, "ambientStrength", light_strength[0]);
      helper::SetShaderFloat(shader_program, "diffuseStrength", light_strength[1]);
      helper::SetShaderFloat(shader_program, "specularStrength", light_strength[2]);
      helper::SetShaderFloat(shader_program, "lightStrength", light_strength[3]);
    };
    SendDataToShader();
    

    glBindVertexArray(box[0]);
    //绑定哪一个VAO
    glDrawArrays(GL_TRIANGLES, 0, 36);//12个顶点
    //绘制模式
    //从数组缓存中的哪一位开始绘制
    //数组中顶点的数量
    glUseProgram(light_shader_program);
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
    model = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera->Zoom),
                                  (float)MyScreenWidth / (float)MyScreenHeight,
                                  0.1f, 100.0f);
    // camera/view transformation
    view = camera->GetViewMatrix();
    model = glm::translate(model, light_position);
    model = glm::scale(model, glm::vec3(0.2f));
    helper::SetShaderMat4(light_shader_program, "projection",projection);
    helper::SetShaderMat4(light_shader_program, "view", view);
    helper::SetShaderMat4(light_shader_program, "model", model);
    //设定光照的位置
    // render boxes
    glBindVertexArray(light);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  helper::exit_program();
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
  if (!can_mouse_move_camera) return;

  if (first_move_mouse) {
    last_x = (float)xpos;
    last_y = (float)ypos;
    first_move_mouse = false;
  }

  float xoffset = (float)xpos - last_x;
  float yoffset =
    last_y -
    (float)ypos;  // reversed since y-coordinates go from bottom to top

  last_x = (float)xpos;
  last_y = (float)ypos;

  camera->ProcessMouseMovement(-xoffset, -yoffset);
}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  if (!can_mouse_move_camera) return;
  std::cout << yoffset << std::endl;
  camera->ProcessMouseScroll((float)yoffset);
}
void ProcessInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera->ProcessKeyboard(helper::Camera::FORWARD, delta_time);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera->ProcessKeyboard(helper::Camera::BACKWARD, delta_time);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera->ProcessKeyboard(helper::Camera::LEFT, delta_time);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera->ProcessKeyboard(helper::Camera::RIGHT, delta_time);
  // if press left alt, then camera can move
  can_mouse_move_camera = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
  if (can_mouse_move_camera) {
    first_move_mouse = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}