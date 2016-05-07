#include "KeySpline.h"
#include <Eigen/Geometry>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>

/**
 * Calculate the lenght of a segment using a Gaussian Quadrature
 * @param  u1 the first time point
 * @param  u2 the second point of the segemnt
 * @param  Gk 4 control poitns describing the spline
 * @return    the length of the segment.
 */
float gaussQuad3(float u1, float u2, const Eigen::MatrixXf & Gk, const Eigen::Matrix4f & B){
    float w[] = {5/9.0,8/9.0,5/9.0};
    float x[] = {-sqrtf(3/5.0),0,sqrtf(3/5.0)};
    float pointSummation = 0;
    for(int i = 0; i < 3; i++){
        float u = (u2-u1)/2.0 * x[i] + (u1 + u2)/2.0;
        Eigen::Vector4f du;
        du << 0,1,2*u,3*u*u;
        pointSummation+= w[i]*(Gk*B*du).norm();
    }
    return pointSummation * (u2-u1)/2.0;
}

//Calculate the fernet frame of a point.
glm::vec3 getNormalVector(const Eigen::Matrix4f & B, const Eigen::Vector3f points[],
    float u){
    Eigen::MatrixXf G(3,4);
    G << points[0](0), points[1](0), points[2](0), points[3](0),
         points[0](1), points[1](1), points[2](1), points[3](1),
         points[0](2), points[1](2), points[2](2), points[3](2);

    Eigen::Vector4f dUVec, d2UVec;
    dUVec << 0.0f, 1.0f, 2*u, 3*u*u;
    dUVec= dUVec.transpose();


    Eigen::Vector3f Tvec = (G*B*dUVec).normalized(); //T Vector
    return glm::vec3(Tvec(0),Tvec(1),Tvec(2));
}

KeySpline::KeySpline():
numSplines(0),
usTableDirty(true),
VertexBuffer(nullptr),
IndexBuffer(nullptr)
{
    setSplineType(CATMULL);
};
KeySpline::~KeySpline(){};

void KeySpline::setSplineType(SplineType t){
    switch (t){
        case CATMULL:
            this->B << 0.0f, -1.0f,  2.0f, -1.0f,
                       2.0f,  0.0f, -5.0f,  3.0f,
                       0.0f,  1.0f,  4.0f, -3.0f,
                       0.0f,  0.0f, -1.0f,  1.0f;
            this->B*=0.5;
            break;
        case BSPLINE:
            this->B  << 1.0f, -3.0f,  3.0f, -1.0f,
                        4.0f,  0.0f, -6.0f,  3.0f,
                        1.0f,  3.0f,  3.0f, -3.0f,
                        0.0f,  0.0f,  0.0f,  1.0f;
            this->B /= 6.0f;
            break;
    }
    if(this->type != t){
        recalculateTable(6);
        this->type = t;
    }
}

void KeySpline::addNode(const SplineNode & node){
    nodePos.push_back(node.getPosition());
    nodeRot.push_back(node.getRotation());
    if(nodePos.size() >= 3){
        std::cout << "Increasing splines" << std::endl;
        numSplines++;
    }
    if(numSplines > 0){
           recalculateTable(6);
    }
    updateVBO = true;
}


void KeySpline::close(){
    nodePos.push_back(nodePos[0]);
    nodeRot.push_back(nodeRot[0]);
    //n-2,n-1,n,0
    nodePos.push_back(nodePos[1]);
    nodeRot.push_back(nodeRot[1]);
    //n-1,n,0,1
    nodePos.push_back(nodePos[2]);
    nodeRot.push_back(nodeRot[2]);
    //n,0,1,2
    numSplines+=3;
    recalculateTable(6);
    updateVBO = true;
}

Transform KeySpline::transformAt(float s) const{
    assert(numSplines > 0);

    float kfloat;
    float uu = s;
    float u = std::modf(uu, &kfloat);
    int k = (int)std::floor(kfloat) % numSplines;
    Eigen::MatrixXf GPos(3,4);
    Eigen::MatrixXf GRot(4,4);
    std::vector<Eigen::Vector3f> cappedPos(nodePos);
    cappedPos.push_back(nodePos.back());
    for(int i = 0; i < 4; i++){
        GPos.block<3,1>(0,i) = cappedPos[k+i];
    }
    Eigen::Vector4f uVec(1.0,u,u*u,u*u*u);
    Eigen::Vector3f P= GPos * B * uVec;
    Transform t;
    t.setPosition(glm::vec3(P(0),P(1),P(2)));
    t.lookAlong(getNormalVector(B,&(cappedPos[k]),u));
    return t;
}


glm::vec3 KeySpline::normalAt(float s) const{
    float kfloat;
    float uu = s;
    float u = std::modf(uu, &kfloat);
    int k = (int)std::floor(kfloat) % numSplines;
    Eigen::MatrixXf GPos(3,4);
    Eigen::MatrixXf GRot(4,4);
    std::vector<Eigen::Vector3f> cappedPos(nodePos);
    cappedPos.push_back(nodePos.back());
   
    return getNormalVector(B,&(cappedPos[k]),u);

}
float KeySpline::sToU(float s)
{
    float sMod = std::fmod(s,usTable.back().second);
    if(usTableDirty)
    {
        recalculateTable(6);
        usTableDirty = false;
    }
    int i = 0;
    float last = 0;
    float cur = 0;

    while( i < (int)usTable.size() && sMod >= cur){
        i++;
        last = cur;
        cur = usTable[i].second;
        
    }
    if(i == 0){
        return 0;
    }
    float interpolationConst = (sMod-last)/(cur-last);
    return (1-interpolationConst)*usTable[i-1].first + interpolationConst*usTable[i].first;

}

void KeySpline::recalculateTable(int discretization){
    if(numSplines == 0)
        return;
    usTable.clear();
    int ncps = (int)nodePos.size();
    Eigen::MatrixXf G(3,ncps + 1);
    Eigen::MatrixXf Gk(3,4);

    usTable.push_back(std::make_pair(0.0f,0.0f));
    float s = 0;
    for(int i = 0; i < ncps; ++i) {
            G.block<3,1>(0,i) = nodePos[i];
    }
    //assign last node duplicate to cap the spline
    G.block<3,1>(0,ncps) = nodePos.back();
    ncps++;
    for(int k = 0; k < ncps - 3; ++k) {
        int n = discretization; // table resolution discretization
            // Gk is the 3x4 block starting at column k
        Gk = G.block<3,4>(0,k);
        for(int i = 0; i < n-1; ++i) {
            float u = i / (n - 1.0f);
            float u2 = u+1.0/(n - 1);
                // Compute spline point at u
            float ds = gaussQuad3(u, u2 , Gk, B);
            usTable.push_back(std::make_pair(u2+k,(s+=ds)));
        }
    }

}

int KeySpline::getNumNodes()
{
    return nodePos.size();
}


SharedPointer<ion::Graphics::IVertexBuffer> KeySpline::GetVertexBuffer()
{
    static SingletonPointer<ion::CGraphicsAPI> GraphicsAPI;

    if(this->VertexBuffer == nullptr)
    {
        this->VertexBuffer = GraphicsAPI->CreateVertexBuffer();
        ion::Graphics::SInputLayoutElement InputLayout[] =
            {
                { "vPosition",  3, ion::Graphics::EAttributeType::Float },
            };
        VertexBuffer->SetInputLayout(InputLayout, ION_ARRAYSIZE(InputLayout));

    }
    if(updateVBO && numSplines > 0){

        Eigen::MatrixXf G(3,nodePos.size() + 1);
        Eigen::MatrixXf Gk(3,4);
        for(int i = 0; i < nodePos.size(); i++){
            G.block<3,1>(0,i) = nodePos[i];
        }
        G.block<3,1>(0,nodePos.size()) = nodePos.back();
        std::vector<glm::vec3>  positions;
        positions.clear();
        for(int k = 0; k < numSplines; k++){
            Gk = G.block<3,4>(0,k);
            int n = 32;
            for(int i = 0; i < n; i++){
                float u = i/(n-1.0f);
                Eigen::Vector4f uVec(1.0,u,u*u,u*u*u);
                Eigen::Vector3f P=Gk*B*uVec;
                positions.push_back(glm::vec3(P(0),P(1),P(2)));
            }
        }
        VertexBuffer->UploadData(positions);
        updateVBO = false;
    }
    return VertexBuffer;
}

SharedPointer<ion::Graphics::IIndexBuffer> KeySpline::GetIndexBuffer()
{
    static SingletonPointer<ion::CGraphicsAPI> GraphicsAPI;

    if(this->IndexBuffer == nullptr)
    {
        IndexBuffer = GraphicsAPI->CreateIndexBuffer();
    }
    vector<u32> IndexData;
    for(int k = 0; k + 1< numSplines * 32; k++)
    {
        IndexData.push_back(k);
        IndexData.push_back(k + 1);
    }
    IndexBuffer->UploadData(IndexData);
    return IndexBuffer;
}








