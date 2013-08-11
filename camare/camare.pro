# =====================================================================================
#  
#        Filename:  camare.pro
# 
#     Description:  项目文件
# 
#         Version:  1.0
#         Created:  2013年06月10日 18时37分01秒
# 
#          Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
#         Company:  Class 1107 of Computer Science and Technology
#  
# =====================================================================================
 
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += /usr/include/opencv
LIBS += -lopencv_core \
	-lopencv_imgproc \
	-lopencv_highgui \
	-lopencv_ml \
	-lopencv_video \
	-lopencv_features2d \
	-lopencv_calib3d \
	-lopencv_objdetect \
	-lopencv_contrib \
	-lopencv_legacy \
	-lopencv_flann

# Input
HEADERS += CamareGet.h
SOURCES += CamareGet.cpp main.cpp
