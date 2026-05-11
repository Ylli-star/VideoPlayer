#include "glad/glad.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string>
#include <GLFW/glfw3.h>
#include "include/Shader.h"

extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
float vertex[]{
  -1.0, 1.0, 0.0, 1.0,/*majtas lart*/
  -1.0, -1.0, 0.0, 0.0,/*majtas poshte*/
  1.0, -1.0, 1.0, 0.0,/*djathtas poshte*/
  1.0, 1.0, 1.0, 1.0/*djathtas lart*/
};
unsigned int indecies[]{
  0,1,2,
  2,3,0
};
void initTex( GLuint* tex, int tex_type = GL_TEXTURE0, int width = 0, int height = 0, int type = GL_UNSIGNED_BYTE){
  glGenTextures(1, tex);
  glActiveTexture(tex_type);
  glBindTexture(GL_TEXTURE_2D, *tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, type, nullptr);
}
void init(GLuint *vbo, GLuint *vao, GLuint *ebo)
{
  glGenVertexArrays(1, vao);
  glBindVertexArray(*vao);

  glGenBuffers(1, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

  glGenBuffers(1, ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indecies), indecies, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);
}


int main(int argc, char** argv) {

    if(argc < 2 || argc > 3){
      std::cerr<<"Gabim ne shkrimin  e arg!";
      return -1;
    }
    if(!glfwInit()){
      std::cerr<<"Gabim ne inicializim glfw";
      return -1;
    }
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    int width = mode->width;
    int height = mode->height;

    GLFWwindow* window = glfwCreateWindow(width, height, "VideoPlayer", NULL, NULL);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      std::cerr<<"GLAD nuk u inicializua!";
      return -1;
    }
    
    glViewport(0, 0, width, height);

    GLuint vbo, vao, ebo;
    init(&vbo, &vao, &ebo);

    AVFormatContext* fc = nullptr;

    if(avformat_open_input(&fc, *(argv+1), nullptr, nullptr) != 0){
      std::cerr<<"Gabim ne hapjen e file!";
      return -1;
    }
    if(avformat_find_stream_info(fc, nullptr) < 0){
      std::cerr<<"Gabim ne gjetjen e stream-it!!";
      return -1;
    }
 
    AVCodecID cID;

    int index = -1;

    for(int i = 0; i < fc->nb_streams; i++){
      if(fc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
        index = i;
        break;
      }
    }
    if(index == -1){
      std::cerr<<"Nuk u gjet video!";
      return -1;
    }
    cID= fc->streams[index]->codecpar->codec_id;
    
    const AVCodec* c = avcodec_find_decoder(cID);

    AVCodecContext* cc = avcodec_alloc_context3(c);

    avcodec_parameters_to_context(cc, fc->streams[index]->codecpar);

    if(avcodec_open2(cc, c, nullptr) != 0){
      std::cerr<<"Gabim nr hapjen e codec!!";
      return -1;
    }

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    GLuint vertex, fragment;

    Shader sh(&vertex, &fragment, "include/vertex.vert", "include/fragment.frag");

    GLuint tex[3];
    
    initTex(&tex[0], GL_TEXTURE0, cc->width, cc->height, GL_UNSIGNED_BYTE);
    initTex(&tex[1], GL_TEXTURE1, cc->width/2, cc->height/2, GL_UNSIGNED_BYTE);
    initTex(&tex[2], GL_TEXTURE2, cc->width/2, cc->height/2, GL_UNSIGNED_BYTE);

    sh.useProgram();
    
    glUniform1i(glGetUniformLocation(sh.programId, "yvalue"), 0);
    glUniform1i(glGetUniformLocation(sh.programId, "uvalue"), 1);
    glUniform1i(glGetUniformLocation(sh.programId, "vvalue"), 2);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    while(av_read_frame(fc, packet) >= 0){
      if(packet->stream_index == index){
        if(avcodec_send_packet(cc, packet)<0){
          std::cerr<<"Gabim ne dergimin e paketave!";
          return -1;
        }
      }
         while (true) {
            int ret = avcodec_receive_frame(cc, frame);

            if (ret == 0) {
                std::cout << "Frame decoded: "<< frame->width << "x"<< frame->height << "\n";

                //vizatim
              if(!(frame->format == AV_PIX_FMT_YUV420P))
                continue;
              glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

              glActiveTexture(GL_TEXTURE0);
              glBindTexture(GL_TEXTURE_2D, tex[0]);
              glPixelStorei(GL_UNPACK_ROW_LENGTH, frame->linesize[0]);
              glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                frame->width, frame->height,
                GL_RED, GL_UNSIGNED_BYTE, frame->data[0]);

              glActiveTexture(GL_TEXTURE1);
              glBindTexture(GL_TEXTURE_2D, tex[1]);
              glPixelStorei(GL_UNPACK_ROW_LENGTH, frame->linesize[1]);
              glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                frame->width/2, frame->height/2,
                GL_RED, GL_UNSIGNED_BYTE, frame->data[1]);

              glActiveTexture(GL_TEXTURE2);
              glBindTexture(GL_TEXTURE_2D, tex[2]);
              glPixelStorei(GL_UNPACK_ROW_LENGTH, frame->linesize[2]);
              glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                frame->width/2, frame->height/2,
                GL_RED, GL_UNSIGNED_BYTE, frame->data[2]);

              glClear(GL_COLOR_BUFFER_BIT);

              glBindVertexArray(vao);

              glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

              glfwSwapBuffers(window);

              glfwPollEvents();
            }
            else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            }
            else {
                std::cerr << "Gabim ne receive_frame\n";
                break;
            }
          }
      av_packet_unref(packet);
    }

    avcodec_send_packet(cc, nullptr);

  while (avcodec_receive_frame(cc, frame) == 0) 
  {
    
  }

    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&cc);
    avformat_close_input(&fc);
  return 0;
}
