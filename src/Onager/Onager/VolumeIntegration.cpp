#include "VolumeIntegration.h"
#include "Shapes.h"
#include <memory>
#include <math.h>

namespace ong
{



	struct FaceIntegrals
	{
		float fa, fb, fc;
		float faa, fbb, fcc;
		float faaa, fbbb, fccc;
		float faab, fbbc, fcca;
	};

	struct ProjectionIntegrals
	{
		float p1;
		float pa, pb;
		float paa, pbb;
		float pab;
		float paaa, pbbb;
		float paab, pabb;
	};

	struct Space
	{
		int A;
		int B;
		int C;
	};



#define SQR(x) ((x)*(x))
#define CUBE(x) ((x)*(x)*(x))


	void computeProjectionIntegrals(const Hull* h, int f, Space* S, ProjectionIntegrals* I)
	{
		float a0, a1, da;
		float b0, b1, db;
		float a0_2, a0_3, a0_4, b0_2, b0_3, b0_4;
		float a1_2, a1_3, b1_2, b1_3;
		float c1, ca, caa, caaa, cb, cbb, cbbb;
		float cab, kab, caab, kaab, cabb, kabb;


		memset(I, 0, sizeof(ProjectionIntegrals));


		HalfEdge* e0 = h->pEdges + h->pFaces[f].edge;
		HalfEdge* e = e0;
		HalfEdge* en;
		do
		{
			en = h->pEdges + e->next;

			a0 = h->pVertices[e->tail][S->A];
			b0 = h->pVertices[e->tail][S->B];
			a1 = h->pVertices[en->tail][S->A];
			b1 = h->pVertices[en->tail][S->B];

			da = a1 - a0;
			db = b1 - b0;

			a0_2 = a0 * a0; a0_3 = a0_2 * a0; a0_4 = a0_3 * a0;
			b0_2 = b0 * b0; b0_3 = b0_2 * b0; b0_4 = b0_3 * b0;
			a1_2 = a1 * a1; a1_3 = a1_2 * a1;
			b1_2 = b1 * b1; b1_3 = b1_2 * b1;

			c1 = a1 + a0;
			ca = a1*c1 + a0_2; caa = a1*ca + a0_3; caaa = a1*caa + a0_4;
			cb = b1*(b1 + b0) + b0_2; cbb = b1*cb + b0_3; cbbb = b1*cbb + b0_4;
			cab = 3 * a1_2 + 2 * a1*a0 + a0_2; kab = a1_2 + 2 * a1*a0 + 3 * a0_2;
			caab = a0*cab + 4 * a1_3; kaab = a1*kab + 4 * a0_3;
			cabb = 4 * b1_3 + 3 * b1_2*b0 + 2 * b1*b0_2 + b0_3;
			kabb = b1_3 + 2 * b1_2*b0 + 3 * b1*b0_2 + 4 * b0_3;

			I->p1 += db*c1;
			I->pa += db*ca;
			I->paa += db*caa;
			I->paaa += db*caaa;
			I->pb += da*cb;
			I->pbb += da*cbb;
			I->pbbb += da*cbbb;
			I->pab += db*(b1*cab + b0*kab);
			I->paab += db*(b1*caab + b0*kaab);
			I->pabb += da*(a1*cabb + a0*kabb);


			e = en;
		} while (e != e0);

		I->p1 /= 2.0f;
		I->pa /= 6.0f;
		I->paa /= 12.0f;
		I->paaa /= 20.0f;
		I->pb /= -6.0f;
		I->pbb /= -12.0f;
		I->pbbb /= -20.0f;
		I->pab /= 24.0f;
		I->paab /= 60.0f;
		I->pabb /= -60.0f;
	}

	void computeFaceIntegrals(const Hull* h, int f, Space* S, FaceIntegrals* I)
	{
		vec3 n;
		float w;
		float k1, k2, k3, k4;

		ProjectionIntegrals pI;
		computeProjectionIntegrals(h, f, S, &pI);

		w = -h->pPlanes[f].d;
		n = h->pPlanes[f].n;

		k1 = 1 / n[S->C]; k2 = k1 * k1; k3 = k2*k1; k4 = k3*k1;

		I->fa = k1 * pI.pa;
		I->fb = k1 * pI.pb;
		I->fc = -k2 * (n[S->A] * pI.pa + n[S->B] * pI.pb + w*pI.p1);


		I->faa = k1 * pI.paa;
		I->fbb = k1 * pI.pbb;
		I->fcc = k3 * (n[S->A] * n[S->A] * pI.paa + 2 * n[S->A] * n[S->B] * pI.pab + n[S->B] * n[S->B] * pI.pbb
			+ w*(2 * (n[S->A] * pI.pa + n[S->B] * pI.pb) + w*pI.p1));

		I->faaa = k1 * pI.paaa;
		I->fbbb = k1 * pI.pbbb;
		I->fccc = -k4 * (CUBE(n[S->A])*pI.paaa + 3 * SQR(n[S->A])*n[S->B] * pI.paab
			+ 3 * n[S->A] * SQR(n[S->B])*pI.pabb + CUBE(n[S->B])*pI.pbbb
			+ 3 * w*(SQR(n[S->A])*pI.paa + 2 * n[S->A] * n[S->B] * pI.pab + SQR(n[S->B])*pI.pbb)
			+ w*w*(3 * (n[S->A] * pI.pa + n[S->B] * pI.pb) + w*pI.p1));

		I->faab = k1 * pI.paab;
		I->fbbc = -k2 * (n[S->A] * pI.pabb + n[S->B] * pI.pbbb + w*pI.pbb);
		I->fcca = k3 * (SQR(n[S->A])*pI.paaa + 2 * n[S->A] * n[S->B] * pI.paab + SQR(n[S->B])*pI.pabb
			+ w*(2 * (n[S->A] * pI.paa + n[S->B] * pI.pab) + w*pI.pa));
	}


	void computeVolumeIntegrals(const Hull* hull, VolumeIntegrals* I)
	{

		static const int X = 0, Y = 1, Z = 2;

		memset(I, 0, sizeof(VolumeIntegrals));

		Face* f;
		Plane* p;
		vec3 n;
		int i;

		for (i = 0; i < hull->numFaces; ++i)
		{
			f = hull->pFaces + i;
			p = hull->pPlanes + i;

			n = vec3(abs(p->n.x), abs(p->n.y), abs(p->n.z));

			Space S;


			if (n.x > n.y && n.x > n.z) S.C = X;
			else S.C = (n.y > n.z) ? Y : Z;
			S.A = (S.C + 1) % 3;
			S.B = (S.A + 1) % 3;

			FaceIntegrals fI;

			computeFaceIntegrals(hull, i, &S, &fI);

			I->t0 += p->n.x * ((S.A == X) ? fI.fa : ((S.B == X) ? fI.fb : fI.fc));

			I->t1[S.A] += p->n[S.A] * fI.faa;
			I->t1[S.B] += p->n[S.B] * fI.fbb;
			I->t1[S.C] += p->n[S.C] * fI.fcc;
			I->t2[S.A] += p->n[S.A] * fI.faaa;
			I->t2[S.B] += p->n[S.B] * fI.fbbb;
			I->t2[S.C] += p->n[S.C] * fI.fccc;
			I->tp[S.A] += p->n[S.A] * fI.faab;
			I->tp[S.B] += p->n[S.B] * fI.fbbc;
			I->tp[S.C] += p->n[S.C] * fI.fcca;

		}

		I->t1[X] /= 2; I->t1[Y] /= 2; I->t1[Z] /= 2;
		I->t2[X] /= 3; I->t2[Y] /= 3; I->t2[Z] /= 3;
		I->tp[X] /= 2; I->tp[Y] /= 2; I->tp[Z] /= 2;
	}


}
