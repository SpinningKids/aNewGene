
/************************************
  REVISION LOG ENTRY
  Revision By: pan
  Revised on 2/16/2002 10:11:37 AM
  Comments: creation
 ************************************/

#ifndef GLTEXTURE_H_
#define GLTEXTURE_H_

class GLTexture {
    float* mem_;
    int size_;
    unsigned int txt_;
    bool init_;
public:
    explicit GLTexture(int logsize);

    ~GLTexture();

    float* getImage() const { return mem_; }
    int getSize() const { return size_; }

    void update();
    void use();
    int getID() const { return txt_; }
};

#endif // GLTEXTURE_H_
