#ifndef LIYT_H
#define LIYT_H

void liyt_genMtxPosRot(Mtx out, float xpos, float ypos, float zpos, float xrot, float yrot, float zrot) {
	Mtx xrotmtx, yrotmtx, zrotmtx;
	guMtxRotRad(xrotmtx, 'x', xrot);
	guMtxRotRad(yrotmtx, 'y', yrot);
	guMtxRotRad(zrotmtx, 'z', zrot);
	guMtxConcat(xrotmtx, yrotmtx, yrotmtx);
	guMtxConcat(yrotmtx, zrotmtx, zrotmtx);
	guMtxTransApply(zrotmtx, out, xpos, ypos, zpos);
	return;
}

void liyt_genMtxPosRotZyx(Mtx out, float xpos, float ypos, float zpos, float xrot, float yrot, float zrot) {
	Mtx xrotmtx, yrotmtx, zrotmtx;
	guMtxRotRad(xrotmtx, 'x', xrot);
	guMtxRotRad(yrotmtx, 'y', yrot);
	guMtxRotRad(zrotmtx, 'z', zrot);
	guMtxConcat(zrotmtx, yrotmtx, yrotmtx);
	guMtxConcat(yrotmtx, xrotmtx, xrotmtx);
	guMtxTransApply(xrotmtx, out, xpos, ypos, zpos);
	return;
}

void c_guMtxConcatTransOnly(const Mtx a,const Mtx b,Mtx ab,float size) {
	Mtx tmp;
	MtxP m;

	if(ab==b || ab==a)
		m = tmp;
	else
		m = ab;

	//m[0][0] = a[0][0]*b[0][0] + a[0][1]*b[1][0] + a[0][2]*b[2][0];
	//m[0][1] = a[0][0]*b[0][1] + a[0][1]*b[1][1] + a[0][2]*b[2][1];
	//m[0][2] = a[0][0]*b[0][2] + a[0][1]*b[1][2] + a[0][2]*b[2][2];
	m[0][0] = size;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = a[0][0]*b[0][3] + a[0][1]*b[1][3] + a[0][2]*b[2][3] + a[0][3];

	//m[1][0] = a[1][0]*b[0][0] + a[1][1]*b[1][0] + a[1][2]*b[2][0];
	//m[1][1] = a[1][0]*b[0][1] + a[1][1]*b[1][1] + a[1][2]*b[2][1];
	//m[1][2] = a[1][0]*b[0][2] + a[1][1]*b[1][2] + a[1][2]*b[2][2];
	m[1][0] = 0.0f;
	m[1][1] = size;
	m[1][2] = 0.0f;
	m[1][3] = a[1][0]*b[0][3] + a[1][1]*b[1][3] + a[1][2]*b[2][3] + a[1][3];

	//m[2][0] = a[2][0]*b[0][0] + a[2][1]*b[1][0] + a[2][2]*b[2][0];
	//m[2][1] = a[2][0]*b[0][1] + a[2][1]*b[1][1] + a[2][2]*b[2][1];
	//m[2][2] = a[2][0]*b[0][2] + a[2][1]*b[1][2] + a[2][2]*b[2][2];
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = a[2][0]*b[0][3] + a[2][1]*b[1][3] + a[2][2]*b[2][3] + a[2][3];

	if(m==tmp)
		c_guMtxCopy(tmp,ab);
}

#endif