/*BHEADER**********************************************************************
 * (c) 1995   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 1.1.1.1 $
 *********************************************************************EHEADER*/
#ifndef _WIN32
#include <sys/param.h>
#endif

#include "pfload_file.h"
#include "load.h"
#include "tools_io.h"


/*-----------------------------------------------------------------------
 * LoadParflowB:
 *-----------------------------------------------------------------------*/

void           LoadParflowB(filename, all_subgrids, background, databox)
char          *filename;
SubgridArray  *all_subgrids;
Background    *background;
Databox       *databox;
{
   char      output_name[MAXPATHLEN];
   FILE     *file;
   FILE     *dist_file;

   Subgrid  *subgrid;

   int       process, num_procs;
   int       p;

   double   *ptr;

   int       NX = DataboxNx(databox);
   int       NY = DataboxNy(databox);
   int       NZ = DataboxNz(databox);

   int  ix, iy, iz;
   int  nx, ny, nz;

   int  k, j, s_i;

   long file_pos=0;


   /*--------------------------------------------------------------------
    * Determine num_procs, and clear output files
    *--------------------------------------------------------------------*/

   num_procs = -1;
   ForSubgridI(s_i, all_subgrids)
   {
      subgrid = SubgridArraySubgrid(all_subgrids, s_i);

      process = SubgridProcess(subgrid);

#ifdef AMPS_SPLIT_FFILE
      sprintf(output_name, "%s.%05d", filename, process);
      remove(output_name);
#endif
	    
      if (process > num_procs)
	 num_procs = process;
   }
   num_procs++;

   /*--------------------------------------------------------------------
    * Load the data
    *--------------------------------------------------------------------*/

#ifndef AMPS_SPLIT_FFILE
   if ((file = fopen(filename, "wb")) == NULL)
   {
      printf("Unable to open outputfile <%s>\n", output_name);
      exit(1);
   }


   strcpy(output_name, filename);
   strcat(output_name, ".dist");

   if ((dist_file = fopen(output_name, "wb")) == NULL)
   {
      printf("Unable to open distribution outputfile <%s>\n", output_name);
      exit(1);
   }


   fprintf(dist_file, "0\n");
#endif

   for(p = 0; p < num_procs; p++)
   {
      ForSubgridI(s_i, all_subgrids)
      {
	 subgrid = SubgridArraySubgrid(all_subgrids, s_i);

	 process = SubgridProcess(subgrid);

	 if(process == p)
	 {

#ifdef AMPS_SPLIT_FFILE
	    sprintf(output_name, "%s.%05d", filename, process);
	    
 	    if ((file = fopen(output_name, "a+")) == NULL)
	    {
	       printf("Unable to open outputfile <%s>\n", output_name);
	       exit(1);
	    }
#endif
	    
	    /* if (process == 0), write header info */
	    if(!process)
	    {
	       tools_WriteDouble(file, &BackgroundX(background),  1);
	       tools_WriteDouble(file, &BackgroundY(background),  1);
	       tools_WriteDouble(file, &BackgroundZ(background),  1);

	       tools_WriteInt(file, &NX, 1);
	       tools_WriteInt(file, &NY, 1);
	       tools_WriteInt(file, &NZ, 1);
	       
	       tools_WriteDouble(file, &BackgroundDX(background),  1);
	       tools_WriteDouble(file, &BackgroundDY(background),  1);
	       tools_WriteDouble(file, &BackgroundDZ(background),  1);
	       
	       tools_WriteInt(file, &num_procs,  1);
	       
	       file_pos += 6*tools_SizeofDouble + 4*tools_SizeofInt;
	    }
	    
	    ix = SubgridIX(subgrid);
	    iy = SubgridIY(subgrid);
	    iz = SubgridIZ(subgrid);
	    
	    nx = SubgridNX(subgrid);
	    ny = SubgridNY(subgrid);
	    nz = SubgridNZ(subgrid);
	    
	    ptr = DataboxCoeff(databox, ix, iy, iz);
	    
	    tools_WriteInt(file, &ix,  1);
	    tools_WriteInt(file, &iy,  1);
	    tools_WriteInt(file, &iz,  1);
	    
	    tools_WriteInt(file, &nx,  1);
	    tools_WriteInt(file, &ny,  1);
	    tools_WriteInt(file, &nz,  1);
	    
	    tools_WriteInt(file, &SubgridRX(subgrid),  1);
	    tools_WriteInt(file, &SubgridRY(subgrid),  1);
	    tools_WriteInt(file, &SubgridRZ(subgrid),  1);
	    
	    for (k=0; k < nz; k++)
	       for (j=0; j < ny; j++)
		  tools_WriteDouble(file, ptr + j*(NX) + k*(NX)*(NY), (nx));
	    
	    file_pos += 9*tools_SizeofInt + (nx*ny*nz) * tools_SizeofDouble;

#ifdef AMPS_SPLIT_FFILE
	    fclose(file);
#else
	    fprintf(dist_file, "%ld\n", file_pos);
#endif

	 }
      }
   }

#ifndef AMPS_SPLIT_FFILE
   fclose(file);
   fclose(dist_file);
#endif
}
