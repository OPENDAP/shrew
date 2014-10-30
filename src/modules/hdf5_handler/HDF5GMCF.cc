// This file is part of the hdf5_handler implementing for the CF-compliant
// Copyright (c) 2011-2013 The HDF Group, Inc. and OPeNDAP, Inc.
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
// You can contact The HDF Group, Inc. at 1800 South Oak Street,
// Suite 203, Champaign, IL 61820  

/////////////////////////////////////////////////////////////////////////////
/// \file HDF5GMCF.cc
/// \brief Implementation of the mapping of NASA generic HDF5 products to DAP by following CF
///
///  It includes functions to 
///  1) retrieve HDF5 metadata info.
///  2) translate HDF5 objects into DAP DDS and DAS by following CF conventions.
///
///
/// \author Muqun Yang <myang6@hdfgroup.org>
///
/// Copyright (C) 2011-2013 The HDF Group
///
/// All rights reserved.

#include "HDF5CF.h"
using namespace HDF5CF;

GMCVar::GMCVar(Var*var) {

    newname = var->newname;
    name = var->name;
    fullpath = var->fullpath;
    rank  = var->rank;
    dtype = var->dtype;
    unsupported_attr_dtype = var->unsupported_attr_dtype;
    unsupported_dspace = var->unsupported_dspace;
    
    for (vector<Attribute*>::iterator ira = var->attrs.begin();
        ira!=var->attrs.end(); ++ira) {
        Attribute* attr= new Attribute();
        attr->name = (*ira)->name;
        attr->newname = (*ira)->newname;
        attr->dtype =(*ira)->dtype;
        attr->count =(*ira)->count;
        attr->strsize = (*ira)->strsize;
        attr->fstrsize = (*ira)->fstrsize;
        attr->value =(*ira)->value;
        attrs.push_back(attr);
    }

    for (vector<Dimension*>::iterator ird = var->dims.begin();
        ird!=var->dims.end(); ++ird) {
        Dimension *dim = new Dimension((*ird)->size);
//cerr <<"dim->name "<< (*ird)->name <<endl;
//cerr <<"dim->newname "<< (*ird)->newname <<endl;
        dim->name = (*ird)->name;
        dim->newname = (*ird)->newname;
        dims.push_back(dim);
    }
    product_type = General_Product;

}
GMSPVar::GMSPVar(Var*var) {

    fullpath = var->fullpath;
    rank  = var->rank;
    unsupported_attr_dtype = var->unsupported_attr_dtype;
    unsupported_dspace = var->unsupported_dspace;
    
    for (vector<Attribute*>::iterator ira = var->attrs.begin();
        ira!=var->attrs.end(); ++ira) {
        Attribute* attr= new Attribute();
        attr->name = (*ira)->name;
        attr->newname = (*ira)->newname;
        attr->dtype =(*ira)->dtype;
        attr->count =(*ira)->count;
        attr->strsize = (*ira)->strsize;
        attr->fstrsize = (*ira)->fstrsize;
        attr->value =(*ira)->value;
        attrs.push_back(attr);
    } // for (vector<Attribute*>::iterator ira = var->attrs.begin();

    for (vector<Dimension*>::iterator ird = var->dims.begin();
        ird!=var->dims.end(); ++ird) {
        Dimension *dim = new Dimension((*ird)->size);
        dim->name = (*ird)->name;
        dim->newname = (*ird)->newname;
        dims.push_back(dim);
    }
}


GMFile::GMFile(const char*path, hid_t file_id, H5GCFProduct product_type, GMPattern gproduct_pattern):
File(path,file_id), product_type(product_type),gproduct_pattern(gproduct_pattern),iscoard(false) 
{


}
GMFile::~GMFile() 
{

    if (!this->cvars.empty()){
        for (vector<GMCVar *>:: const_iterator i= this->cvars.begin(); i!=this->cvars.end(); ++i) {
           delete *i;
        }
    }

    if (!this->spvars.empty()){
        for (vector<GMSPVar *>:: const_iterator i= this->spvars.begin(); i!=this->spvars.end(); ++i) {
           delete *i;
        }
    }

}

string GMFile::get_CF_string(string s) {

    if ((General_Product == product_type &&  OTHERGMS == gproduct_pattern) || s[0] !='/') 
        return File::get_CF_string(s);
    else {
        s.erase(0,1);
        return File::get_CF_string(s);
    }
}

void GMFile::Retrieve_H5_Info(const char *path,
                              hid_t file_id, bool include_attr) throw (Exception) {

    // MeaSure SeaWiFS and Ozone need the attribute info. to build the dimension name list.
    // GPM needs the attribute info. to obtain the lat/lon.
    // So set the include_attr to be true for these products.
    if (product_type == Mea_SeaWiFS_L2 || product_type == Mea_SeaWiFS_L3
        || GPMS_L3 == product_type  || GPMM_L3 == product_type || GPM_L1 == product_type || OBPG_L3 == product_type
        || Mea_Ozone == product_type || General_Product == product_type)  
        File::Retrieve_H5_Info(path,file_id,true);
    else 
        File::Retrieve_H5_Info(path,file_id,include_attr);
}

void GMFile::Retrieve_H5_Supported_Attr_Values() throw (Exception) {

    File::Retrieve_H5_Supported_Attr_Values();
    for (vector<GMCVar *>::iterator ircv = this->cvars.begin();
          ircv != this->cvars.end(); ++ircv) {
          
        //if ((CV_EXIST == (*ircv)->cvartype ) || (CV_MODIFY == (*ircv)->cvartype) 
         //   || (CV_FILLINDEX == (*ircv)->cvartype)){
        if ((*ircv)->cvartype != CV_NONLATLON_MISS){
            for (vector<Attribute *>::iterator ira = (*ircv)->attrs.begin();
                 ira != (*ircv)->attrs.end(); ++ira) {
                Retrieve_H5_Attr_Value(*ira,(*ircv)->fullpath);
            }
        }
    }
    for (vector<GMSPVar *>::iterator irspv = this->spvars.begin();
          irspv != this->spvars.end(); ++irspv) {
          
        for (vector<Attribute *>::iterator ira = (*irspv)->attrs.begin();
              ira != (*irspv)->attrs.end(); ++ira) {
            Retrieve_H5_Attr_Value(*ira,(*irspv)->fullpath);
            Adjust_H5_Attr_Value(*ira);
        }
    }
}

void GMFile::Adjust_H5_Attr_Value(Attribute *attr) throw (Exception) {

    if (product_type == ACOS_L2S) {
        if (("Type" == attr->name) && (H5VSTRING == attr->dtype)) {
            string orig_attrvalues(attr->value.begin(),attr->value.end());
            if (orig_attrvalues != "Signed64") return;
            string new_attrvalues = "Signed32";
            // Since the new_attrvalues size is the same as the orig_attrvalues size
            // No need to adjust the strsize and fstrsize. KY 2011-2-1
            attr->value.clear();
            attr->value.resize(new_attrvalues.size());
            copy(new_attrvalues.begin(),new_attrvalues.end(),attr->value.begin()); 
        }
    } // if (product_type == ACOS_L2S)
}

void GMFile:: Handle_Unsupported_Dtype(bool include_attr) throw(Exception) {

    File::Handle_Unsupported_Dtype(include_attr);
    for (vector<GMCVar *>::iterator ircv = this->cvars.begin();
                ircv != this->cvars.end(); ++ircv) {
        if (true == include_attr) {
            for (vector<Attribute *>::iterator ira = (*ircv)->attrs.begin();
                 ira != (*ircv)->attrs.end(); ++ira) {
                H5DataType temp_dtype = (*ira)->getType();
                if (false == HDF5CFUtil::cf_strict_support_type(temp_dtype)) {
                    delete (*ira);
                    (*ircv)->attrs.erase(ira);
                     ira--;
                }
            }
        }
        H5DataType temp_dtype = (*ircv)->getType();
        if (false == HDF5CFUtil::cf_strict_support_type(temp_dtype)) {
            
            // This may need to be checked carefully in the future,
            // My current understanding is that the coordinate variable can
            // be ignored if the corresponding variable is ignored. 
            // Currently we don't find any NASA files in this category.
            // KY 2012-5-21
            delete (*ircv);
            this->cvars.erase(ircv);
            ircv--;
        }
    } // for (vector<GMCVar *>::iterator ircv = this->cvars.begin();
    for (vector<GMSPVar *>::iterator ircv = this->spvars.begin();
                ircv != this->spvars.end(); ++ircv) {

        if (true == include_attr) {
            for (vector<Attribute *>::iterator ira = (*ircv)->attrs.begin();
                ira != (*ircv)->attrs.end(); ++ira) {
                H5DataType temp_dtype = (*ira)->getType();
                if (false == HDF5CFUtil::cf_strict_support_type(temp_dtype)) {
                    delete (*ira);
                    (*ircv)->attrs.erase(ira);
                    ira--;
                }
            }
        }
        H5DataType temp_dtype = (*ircv)->getType();
        if (false == HDF5CFUtil::cf_strict_support_type(temp_dtype)) {
            delete (*ircv);
            this->spvars.erase(ircv);
            ircv--;
        }
    }// for (vector<GMSPVar *>::iterator ircv = this->spvars.begin();
}

void GMFile:: Handle_Unsupported_Dspace() throw(Exception) {

    File::Handle_Unsupported_Dspace();
    
    if(true == this->unsupported_var_dspace) {
        for (vector<GMCVar *>::iterator ircv = this->cvars.begin();
                ircv != this->cvars.end(); ++ircv) {
            if (true  == (*ircv)->unsupported_dspace ) {
            
                // This may need to be checked carefully in the future,
                // My current understanding is that the coordinate variable can
                // be ignored if the corresponding variable is ignored. 
                // Currently we don't find any NASA files in this category.
                // KY 2012-5-21
                delete (*ircv);
                this->cvars.erase(ircv);
                ircv--;
            }
        } // for (vector<GMCVar *>::iterator ircv = this->cvars.begin();

        for (vector<GMSPVar *>::iterator ircv = this->spvars.begin();
                ircv != this->spvars.end(); ++ircv) {

            if (true == (*ircv)->unsupported_dspace) {
                delete (*ircv);
                this->spvars.erase(ircv);
                ircv--;
            }
        }// for (vector<GMSPVar *>::iterator ircv = this->spvars.begin();
    }// if(true == this->unsupported_dspace) 
}

void GMFile::Add_Dim_Name() throw(Exception){
    
    switch(product_type) {
        case Mea_SeaWiFS_L2:
        case Mea_SeaWiFS_L3:
            Add_Dim_Name_Mea_SeaWiFS();
            break;
        case Aqu_L3:
            Add_Dim_Name_Aqu_L3();
            break;
        case SMAP:
            Add_Dim_Name_SMAP();
            break;
        case ACOS_L2S:
            Add_Dim_Name_ACOS_L2S();
            break;
        case Mea_Ozone:
            Add_Dim_Name_Mea_Ozonel3z();
            break;
        case GPMS_L3:
        case GPMM_L3:
        case GPM_L1:
            Add_Dim_Name_GPM();
        case OBPG_L3:
            Add_Dim_Name_OBPG_L3();
        case General_Product:
            Add_Dim_Name_General_Product();
            break;
        default:
           throw1("Cannot generate dim. names for unsupported datatype");
    } // switch(product_type)

// Just for debugging
#if 0
for (vector<Var*>::iterator irv2 = this->vars.begin();
    irv2 != this->vars.end(); irv2++) {
    for (vector<Dimension *>::iterator ird = (*irv2)->dims.begin();
         ird !=(*irv2)->dims.end(); ird++) {
         cerr<<"Dimension name afet Add_Dim_Name "<<(*ird)->newname <<endl;
    }
}
#endif

}

void GMFile::Add_Dim_Name_OBPG_L3() throw(Exception) {

    Add_Dim_Name_General_Product();

}
void GMFile::Add_Dim_Name_Mea_SeaWiFS() throw(Exception){

//cerr<<"coming to Add_Dim_Name_Mea_SeaWiFS"<<endl;
    
    pair<set<string>::iterator,bool> setret;
    if (Mea_SeaWiFS_L3 == product_type) 
        iscoard = true;
    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {
        Handle_UseDimscale_Var_Dim_Names_Mea_SeaWiFS_Ozone((*irv));
        for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
            ird !=(*irv)->dims.end();++ird) { 
            setret = dimnamelist.insert((*ird)->name);
            if (true == setret.second) 
                Insert_One_NameSizeMap_Element((*ird)->name,(*ird)->size);
        }
    } // for (vector<Var *>::iterator irv = this->vars.begin();
 
    if (true == dimnamelist.empty()) 
        throw1("This product should have the dimension names, but no dimension names are found");
}    


void GMFile::Handle_UseDimscale_Var_Dim_Names_Mea_SeaWiFS_Ozone(Var* var)
throw(Exception){

    Attribute* dimlistattr = NULL;
    bool has_dimlist = false;
    bool has_class = false;
    bool has_reflist = false;

    for(vector<Attribute *>::iterator ira = var->attrs.begin();
          ira != var->attrs.end();ira++) {
        if ("DIMENSION_LIST" == (*ira)->name) {
           dimlistattr = *ira;
           has_dimlist = true;  
        }
        if ("CLASS" == (*ira)->name) 
            has_class = true;
        if ("REFERENCE_LIST" == (*ira)->name) 
            has_reflist = true;
        
        if (true == has_dimlist) 
            break;
        if (true == has_class && true == has_reflist) 
            break; 
    } // for(vector<Attribute *>::iterator ira = var->attrs.begin(); ...

    if (true == has_dimlist) 
        Add_UseDimscale_Var_Dim_Names_Mea_SeaWiFS_Ozone(var,dimlistattr);

    // Dim name is the same as the variable name for dimscale variable
    else if(true == has_class && true == has_reflist) {
        if (var->dims.size() !=1) 
           throw2("dimension scale dataset must be 1 dimension, this is not true for variable ",
                  var->name);

        // The var name is the object name, however, we would like the dimension name to be full path.
        // so that the dim name can be served as the key for future handling.
        (var->dims)[0]->name = var->fullpath;
        (var->dims)[0]->newname = var->fullpath;
        pair<set<string>::iterator,bool> setret;
        setret = dimnamelist.insert((var->dims)[0]->name);
        if (true == setret.second) 
            Insert_One_NameSizeMap_Element((var->dims)[0]->name,(var->dims)[0]->size);
    }

    // No dimension, add fake dim names, this may never happen for MeaSure
    // but just for coherence and completeness.
    // For Fake dimesnion
    else {

        set<hsize_t> fakedimsize;
        pair<set<hsize_t>::iterator,bool> setsizeret;
        for (vector<Dimension *>::iterator ird= var->dims.begin();
            ird != var->dims.end(); ++ird) {
                Add_One_FakeDim_Name(*ird);
                setsizeret = fakedimsize.insert((*ird)->size);
                if (false == setsizeret.second)   
                    Adjust_Duplicate_FakeDim_Name(*ird);
        }
// Just for debugging
#if 0
        for (int i = 0; i < var->dims.size(); ++i) {
            Add_One_FakeDim_Name((var->dims)[i]);
            bool gotoMainLoop = false;
                for (int j =i-1; j>=0 && !gotoMainLoop; --j) {
                    if (((var->dims)[i])->size == ((var->dims)[j])->size){
                        Adjust_Duplicate_FakeDim_Name((var->dims)[i]);
                        gotoMainLoop = true;
                    }
                }
        }
#endif
        
    }
}

void GMFile::Add_UseDimscale_Var_Dim_Names_Mea_SeaWiFS_Ozone(Var *var,Attribute*dimlistattr) 
throw (Exception){
    
    ssize_t objnamelen = -1;
    hobj_ref_t rbuf;
    //hvl_t *vlbuf = NULL;
    vector<hvl_t> vlbuf;
    
    hid_t dset_id = -1;
    hid_t attr_id = -1;
    hid_t atype_id = -1;
    hid_t amemtype_id = -1;
    hid_t aspace_id = -1;
    hid_t ref_dset = -1;


    if(NULL == dimlistattr) 
        throw2("Cannot obtain the dimension list attribute for variable ",var->name);

    if (0==var->rank) 
        throw2("The number of dimension should NOT be 0 for the variable ",var->name);
    
    try {

        //vlbuf = new hvl_t[var->rank];
        vlbuf.resize(var->rank);
    
        hid_t dset_id = H5Dopen(this->fileid,(var->fullpath).c_str(),H5P_DEFAULT);
        if (dset_id < 0) 
            throw2("Cannot open the dataset ",var->fullpath);

        attr_id = H5Aopen(dset_id,(dimlistattr->name).c_str(),H5P_DEFAULT);
        if (attr_id <0 ) 
            throw4("Cannot open the attribute ",dimlistattr->name," of HDF5 dataset ",var->fullpath);

        atype_id = H5Aget_type(attr_id);
        if (atype_id <0) 
            throw4("Cannot obtain the datatype of the attribute ",dimlistattr->name," of HDF5 dataset ",var->fullpath);

        amemtype_id = H5Tget_native_type(atype_id, H5T_DIR_ASCEND);

        if (amemtype_id < 0) 
            throw2("Cannot obtain the memory datatype for the attribute ",dimlistattr->name);


        if (H5Aread(attr_id,amemtype_id,&vlbuf[0]) <0)  
            throw2("Cannot obtain the referenced object for the variable ",var->name);
        

        vector<char> objname;
        int vlbuf_index = 0;

        // The dimension names of variables will be the HDF5 dataset names dereferenced from the DIMENSION_LIST attribute.
        for (vector<Dimension *>::iterator ird = var->dims.begin();
                ird != var->dims.end(); ++ird) {

            rbuf =((hobj_ref_t*)vlbuf[vlbuf_index].p)[0];
            if ((ref_dset = H5Rdereference(attr_id, H5R_OBJECT, &rbuf)) < 0) 
                throw2("Cannot dereference from the DIMENSION_LIST attribute  for the variable ",var->name);

            if ((objnamelen= H5Iget_name(ref_dset,NULL,0))<=0) 
                throw2("Cannot obtain the dataset name dereferenced from the DIMENSION_LIST attribute  for the variable ",var->name);
            objname.resize(objnamelen+1);
            if ((objnamelen= H5Iget_name(ref_dset,&objname[0],objnamelen+1))<=0) 
                throw2("Cannot obtain the dataset name dereferenced from the DIMENSION_LIST attribute  for the variable ",var->name);

            string objname_str = string(objname.begin(),objname.end());
            string trim_objname = objname_str.substr(0,objnamelen);
            (*ird)->name = string(trim_objname.begin(),trim_objname.end());

            pair<set<string>::iterator,bool> setret;
            setret = dimnamelist.insert((*ird)->name);
            if (true == setret.second) 
               Insert_One_NameSizeMap_Element((*ird)->name,(*ird)->size);
            (*ird)->newname = (*ird)->name;
            H5Dclose(ref_dset);
            ref_dset = -1;
            objname.clear();
            vlbuf_index++;
        }// for (vector<Dimension *>::iterator ird = var->dims.begin()
        if(vlbuf.size()!= 0) {

            if ((aspace_id = H5Aget_space(attr_id)) < 0)
                throw2("Cannot get hdf5 dataspace id for the attribute ",dimlistattr->name);

            if (H5Dvlen_reclaim(amemtype_id,aspace_id,H5P_DEFAULT,(void*)&vlbuf[0])<0) 
                throw2("Cannot successfully clean up the variable length memory for the variable ",var->name);

            H5Sclose(aspace_id);
           
        }

        H5Tclose(atype_id);
        H5Tclose(amemtype_id);
        H5Aclose(attr_id);
        H5Dclose(dset_id);
    
       // if(vlbuf != NULL)
        //  delete[] vlbuf;
    }

    catch(...) {

        if(atype_id != -1)
            H5Tclose(atype_id);

        if(amemtype_id != -1)
            H5Tclose(amemtype_id);

        if(aspace_id != -1)
            H5Sclose(aspace_id);

        if(attr_id != -1)
            H5Aclose(attr_id);

        if(dset_id != -1)
            H5Dclose(dset_id);

        //if(vlbuf != NULL)
         //   delete[] vlbuf;

        //throw1("Error in method GMFile::Add_UseDimscale_Var_Dim_Names_Mea_SeaWiFS_Ozone"); 
        throw;
    }
 
}

void GMFile::Add_Dim_Name_Mea_Ozonel3z() throw(Exception){

    iscoard = true;
    bool use_dimscale = false;

    for (vector<Group *>::iterator irg = this->groups.begin();
        irg != this->groups.end(); ++ irg) {
        if ("/Dimensions" == (*irg)->path) {
            use_dimscale = true;
            break;
        }
    }

    if (false == use_dimscale) {

        bool has_dimlist = false;
        bool has_class = false;
        bool has_reflist = false;

        for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); irv++) {

            for(vector<Attribute *>::iterator ira = (*irv)->attrs.begin();
                ira != (*irv)->attrs.end();ira++) {
                if ("DIMENSION_LIST" == (*ira)->name) 
                    has_dimlist = true;  
            }
            if (true == has_dimlist) 
                break;
        }

        if (true == has_dimlist) {
            for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); irv++) {

                for(vector<Attribute *>::iterator ira = (*irv)->attrs.begin();
                    ira != (*irv)->attrs.end();ira++) {
                    if ("CLASS" == (*ira)->name) 
                        has_class = true;
                    if ("REFERENCE_LIST" == (*ira)->name) 
                        has_reflist = true;
                    if (true == has_class && true == has_reflist) 
                        break; 
                } 

                if (true == has_class && 
                    true == has_reflist) 
                    break;
            
            } 
            if (true == has_class && true == has_reflist) 
                use_dimscale = true;
        } // if (true == has_dimlist)
    } // if (false == use_dimscale)

    if (true == use_dimscale) {

        pair<set<string>::iterator,bool> setret;
        for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {
            Handle_UseDimscale_Var_Dim_Names_Mea_SeaWiFS_Ozone((*irv));
            for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                ird !=(*irv)->dims.end();++ird) { 
                setret = dimnamelist.insert((*ird)->name);
                if(true == setret.second) 
                    Insert_One_NameSizeMap_Element((*ird)->name,(*ird)->size);
            }
        }
 
        if (true == dimnamelist.empty()) 
            throw1("This product should have the dimension names, but no dimension names are found");
    } // if (true == use_dimscale)    

    else {

        // Since the dim. size of each dimension of 2D lat/lon may be the same, so use multimap.
        multimap<hsize_t,string> ozonedimsize_to_dimname;
        pair<multimap<hsize_t,string>::iterator,multimap<hsize_t,string>::iterator> mm_er_ret;
        multimap<hsize_t,string>::iterator irmm;
 
        for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {
            bool is_cv = check_cv((*irv)->name);
            if (true == is_cv) {
                if ((*irv)->dims.size() != 1)
                    throw3("The coordinate variable", (*irv)->name," must be one dimension for the zonal average product");
                ozonedimsize_to_dimname.insert(pair<hsize_t,string>(((*irv)->dims)[0]->size,(*irv)->fullpath));
#if 0
            ((*irv)->dims[0])->name = (*irv)->name;
            ((*irv)->dims[0])->newname = (*irv)->name;
            pair<set<string>::iterator,bool> setret;
            setret = dimnamelist.insert(((*irv)->dims[0])->name);
            if (setret.second) 
                Insert_One_NameSizeMap_Element(((*irv)->dims[0])->name,((*irv)->dims[0])->size);
#endif
            }
        }// for (vector<Var *>::iterator irv = this->vars.begin(); ...

        set<hsize_t> fakedimsize;
        pair<set<hsize_t>::iterator,bool> setsizeret;
        pair<set<string>::iterator,bool> setret;
        pair<set<string>::iterator,bool> tempsetret;
        set<string> tempdimnamelist;
        bool fakedimflag = false;

        for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {

            for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                ird != (*irv)->dims.end(); ++ird) {

                fakedimflag = true;
                mm_er_ret = ozonedimsize_to_dimname.equal_range((*ird)->size);
                for (irmm = mm_er_ret.first; irmm!=mm_er_ret.second;irmm++) {
                    setret = tempdimnamelist.insert(irmm->second);
                    if (true == setret.second) {
                       (*ird)->name = irmm->second;
                       (*ird)->newname = (*ird)->name;
                       setret = dimnamelist.insert((*ird)->name);
                       if(setret.second) Insert_One_NameSizeMap_Element((*ird)->name,(*ird)->size);
                       fakedimflag = false;
                       break;
                    }
                }
                      
                if (true == fakedimflag) {
                     Add_One_FakeDim_Name(*ird);
                     setsizeret = fakedimsize.insert((*ird)->size);
                     if (false == setsizeret.second)  
                        Adjust_Duplicate_FakeDim_Name(*ird);
                }
            
            } // for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
            tempdimnamelist.clear();
            fakedimsize.clear();
        } // for (vector<Var *>::iterator irv = this->vars.begin();
    } // else
}

bool GMFile::check_cv(string & varname) throw(Exception) {

     const string lat_name ="Latitude";
     const string time_name ="Time";
     const string ratio_pressure_name ="MixingRatioPressureLevels";
     const string profile_pressure_name ="ProfilePressureLevels";
     const string wave_length_name ="Wavelength";

     if (lat_name == varname) 
        return true;
     else if (time_name == varname) 
        return true;
     else if (ratio_pressure_name == varname) 
        return true;
     else if (profile_pressure_name == varname) 
        return true;
     else if (wave_length_name == varname)
        return true;
     else 
        return false;
}

void GMFile::Add_Dim_Name_GPM()throw(Exception)
{

    // This is used to create a dimension name set.
    pair<set<string>::iterator,bool> setret;

    // One GPM variable (sunVectorInBodyFrame) misses an element for DimensionNames attributes.
    // We need to create a fakedim name to fill in. To make the dimension name unique, we use a counter.
    // int dim_count = 0;
    // map<string,string> varname_to_fakedim;
    // map<int,string> gpm_dimsize_to_fakedimname;

    // We find that GPM has an attribute DimensionNames(nlon,nlat) in this case.
    // We will use this attribute to specify the dimension names.
    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); irv++) {

        bool has_dim_name_attr = false;

        for (vector<Attribute *>::iterator ira = (*irv)->attrs.begin();
                        ira != (*irv)->attrs.end(); ++ira) {

            if("DimensionNames" == (*ira)->name) {

                Retrieve_H5_Attr_Value(*ira,(*irv)->fullpath);
                string dimname_value((*ira)->value.begin(),(*ira)->value.end());

                vector<string> ind_elems;
                char sep=',';
                HDF5CFUtil::Split(&dimname_value[0],sep,ind_elems);

                if(ind_elems.size() != (size_t)((*irv)->getRank())) {
                    throw2("The number of dims obtained from the <DimensionNames> attribute is not equal to the rank ", 
                           (*irv)->name); 
                }

                for(unsigned int i = 0; i<ind_elems.size(); ++i) {

                    ((*irv)->dims)[i]->name = ind_elems[i];
                    // Generate a dimension name is the dimension name is missing.
                    // The routine will ensure that the fakeDim name is unique.
                    if(((*irv)->dims)[i]->name==""){ 
                        Add_One_FakeDim_Name(((*irv)->dims)[i]);
// For debugging
#if 0
                        string fakedim = "FakeDim";
                        stringstream sdim_count;
                        sdim_count << dim_count;
                        fakedim = fakedim + sdim_count.str();
                        dim_count++;
                        ((*irv)->dims)[i]->name = fakedim;
                        ((*irv)->dims)[i]->newname = fakedim;
                        ind_elems[i] = fakedim;
#endif
                    }
                    
                    else {
                        ((*irv)->dims)[i]->newname = ind_elems[i];
                        setret = dimnamelist.insert(((*irv)->dims)[i]->name);
                   
                        if (true == setret.second) {
                            Insert_One_NameSizeMap_Element(((*irv)->dims)[i]->name,
                                                           ((*irv)->dims)[i]->size);
                        }
                        else {
                            if(dimname_to_dimsize[((*irv)->dims)[i]->name] !=((*irv)->dims)[i]->size)
                                throw5("Dimension ",((*irv)->dims)[i]->name, "has two sizes",   
                                        ((*irv)->dims)[i]->size,dimname_to_dimsize[((*irv)->dims)[i]->name]);

                        }
                    }

                }
                has_dim_name_attr = true;
                break;

            }
        }

#if 0
        if(false == has_dim_name_attr) {

            throw4( "The variable ", (*irv)->name, " doesn't have the DimensionNames attribute.",
                    "We currently don't support this case. Please report to the NASA data center.");
        }
            
#endif
    }
 
}
     
void GMFile::Add_Dim_Name_Aqu_L3()throw(Exception)
{
    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); irv++) {
        if ("l3m_data" == (*irv)->name) {
           ((*irv)->dims)[0]->name = "lat";
           ((*irv)->dims)[0]->newname = "lat";
           ((*irv)->dims)[1]->name = "lon";
           ((*irv)->dims)[1]->newname = "lon";
           break;
        }
       
// For the time being, don't assign dimension names to palette,
// we will see if tools can pick up l3m and then make decisions.
#if 0
        if ("palette" == (*irv)->name) {
//cerr <<"coming to palette" <<endl;
          ((*irv)->dims)[0]->name = "paldim0";
           ((*irv)->dims)[0]->newname = "paldim0";
           ((*irv)->dims)[1]->name = "paldim1";
           ((*irv)->dims)[1]->newname = "paldim1";
        }
#endif

    }
}
void GMFile::Add_Dim_Name_SMAP()throw(Exception){

    string tempvarname ="";
    string key = "_lat";
    string smapdim0 ="YDim";
    string smapdim1 ="XDim";

    // Since the dim. size of each dimension of 2D lat/lon may be the same, so use multimap.
    multimap<hsize_t,string> smapdimsize_to_dimname;
    pair<multimap<hsize_t,string>::iterator,multimap<hsize_t,string>::iterator> mm_er_ret;
    multimap<hsize_t,string>::iterator irmm; 

    // Generate dimension names based on the size of "???_lat"(one coordinate variable) 
    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {
        tempvarname = (*irv)->name;
        if ((tempvarname.size() > key.size())&& 
            (key == tempvarname.substr(tempvarname.size()-key.size(),key.size()))){
//cerr<<"tempvarname " <<tempvarname <<endl;
            if ((*irv)->dims.size() !=2) 
                throw1("Currently only 2D lat/lon is supported for SMAP");
            smapdimsize_to_dimname.insert(pair<hsize_t,string>(((*irv)->dims)[0]->size,smapdim0));
            smapdimsize_to_dimname.insert(pair<hsize_t,string>(((*irv)->dims)[1]->size,smapdim1));
            break;
        }
    }

    set<hsize_t> fakedimsize;
    pair<set<hsize_t>::iterator,bool> setsizeret;
    pair<set<string>::iterator,bool> setret;
    pair<set<string>::iterator,bool> tempsetret;
    set<string> tempdimnamelist;
    bool fakedimflag = false;


    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {

        for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
            ird != (*irv)->dims.end(); ++ird) {

            fakedimflag = true;
            mm_er_ret = smapdimsize_to_dimname.equal_range((*ird)->size);
            for (irmm = mm_er_ret.first; irmm!=mm_er_ret.second;irmm++) {
                setret = tempdimnamelist.insert(irmm->second);
                if (setret.second) {
                   (*ird)->name = irmm->second;
                   (*ird)->newname = (*ird)->name;
                   setret = dimnamelist.insert((*ird)->name);
                   if(setret.second) Insert_One_NameSizeMap_Element((*ird)->name,(*ird)->size);
                   fakedimflag = false;
                   break;
                }
            }
                      
            if (true == fakedimflag) {
                 Add_One_FakeDim_Name(*ird);
                 setsizeret = fakedimsize.insert((*ird)->size);
                 if (!setsizeret.second)  
                    Adjust_Duplicate_FakeDim_Name(*ird);
            }
        } // for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
        tempdimnamelist.clear();
        fakedimsize.clear();
    } // for (vector<Var *>::iterator irv = this->vars.begin();
}

void GMFile::Add_Dim_Name_ACOS_L2S()throw(Exception){

    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {

        set<hsize_t> fakedimsize;
        pair<set<hsize_t>::iterator,bool> setsizeret;
        for (vector<Dimension *>::iterator ird= (*irv)->dims.begin();
            ird != (*irv)->dims.end(); ++ird) {
                Add_One_FakeDim_Name(*ird);
                setsizeret = fakedimsize.insert((*ird)->size);
                if (false == setsizeret.second)   
                    Adjust_Duplicate_FakeDim_Name(*ird);
        }
    } // for (vector<Var *>::iterator irv = this->vars.begin();
}
void GMFile::Add_Dim_Name_General_Product()throw(Exception){

    // Check attributes 
    Check_General_Product_Pattern();

    // This general product should follow the HDF5 dimension scale model. 
    if (GENERAL_DIMSCALE == this->gproduct_pattern)
        Add_Dim_Name_Dimscale_General_Product();

}

void GMFile::Check_General_Product_Pattern() throw(Exception) {

    bool has_dimlist = false;
    bool has_dimscalelist  = false;

    // Check if containing the "DIMENSION_LIST" attribute;
    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {
        for(vector<Attribute *>::iterator ira = (*irv)->attrs.begin();
          ira != (*irv)->attrs.end();ira++) {
           if ("DIMENSION_LIST" == (*ira)->name) {
                has_dimlist = true;
                break;
           }
        }
        if (true == has_dimlist)
            break;
    }

    // Check if containing both the attribute "CLASS" and the attribute "REFERENCE_LIST" for the same variable.
    // This is the dimension scale. 
    // Actually "REFERENCE_LIST" is not necessary for a dimension scale dataset. If a dimension scale doesn't
    // have a "REFERENCE_LIST", it is still valid. But no other variables use this dimension scale. We found
    // such a case in a matched_airs_aqua product. KY 2012-12-03
    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {


        for(vector<Attribute *>::iterator ira = (*irv)->attrs.begin();
          ira != (*irv)->attrs.end();ira++) {
            if ("CLASS" == (*ira)->name) {

                Retrieve_H5_Attr_Value(*ira,(*irv)->fullpath);
                string class_value;
                class_value.resize((*ira)->value.size());
                copy((*ira)->value.begin(),(*ira)->value.end(),class_value.begin());

                // Compare the attribute "CLASS" value with "DIMENSION_SCALE". We only compare the string with the size of
                // "DIMENSION_SCALE", which is 15.
                if (0 == class_value.compare(0,15,"DIMENSION_SCALE")) {
                    has_dimscalelist = true;
                    break;
                }
            }
        }

        if (true == has_dimscalelist)
            break;
        
    }

    if (true == has_dimlist && true == has_dimscalelist)
        this->gproduct_pattern = GENERAL_DIMSCALE;

}

void GMFile::Add_Dim_Name_Dimscale_General_Product() throw(Exception) {

    //cerr<<"coming to Add_Dim_Name_Dimscale_General_Product"<<endl;
    pair<set<string>::iterator,bool> setret;
    this->iscoard = true;

    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {
        Handle_UseDimscale_Var_Dim_Names_General_Product((*irv));
        for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
            ird !=(*irv)->dims.end();++ird) { 
            setret = dimnamelist.insert((*ird)->name);
            if (true == setret.second) 
                Insert_One_NameSizeMap_Element((*ird)->name,(*ird)->size);
        }
    } // for (vector<Var *>::iterator irv = this->vars.begin();
 
    if (true == dimnamelist.empty()) 
        throw1("This product should have the dimension names, but no dimension names are found");

}

void GMFile::Handle_UseDimscale_Var_Dim_Names_General_Product(Var *var) throw(Exception) {

    Attribute* dimlistattr = NULL;
    bool has_dimlist = false;
    bool has_dimclass   = false;

    for(vector<Attribute *>::iterator ira = var->attrs.begin();
          ira != var->attrs.end();ira++) {
        if ("DIMENSION_LIST" == (*ira)->name) {
            dimlistattr = *ira;
            has_dimlist = true;  
        }
        if ("CLASS" == (*ira)->name) {

            Retrieve_H5_Attr_Value(*ira,var->fullpath);
            string class_value;
            class_value.resize((*ira)->value.size());
            copy((*ira)->value.begin(),(*ira)->value.end(),class_value.begin());

            // Compare the attribute "CLASS" value with "DIMENSION_SCALE". We only compare the string with the size of
            // "DIMENSION_SCALE", which is 15.
            if (0 == class_value.compare(0,15,"DIMENSION_SCALE")) {
                has_dimclass = true;
                break;
            }
        }

    } // for(vector<Attribute *>::iterator ira = var->attrs.begin(); ...

    // This is a general variable, we need to find the corresponding coordinate variables.
    if (true == has_dimlist) 
        Add_UseDimscale_Var_Dim_Names_General_Product(var,dimlistattr);

    // Dim name is the same as the variable name for dimscale variable
    else if(true == has_dimclass) {
        if (var->dims.size() !=1) 
           throw2("Currently dimension scale dataset must be 1 dimension, this is not true for the dataset ",
                  var->name);

        // The var name is the object name, however, we would like the dimension name to be the full path.
        // so that the dim name can be served as the key for future handling.
        (var->dims)[0]->name = var->fullpath;
        (var->dims)[0]->newname = var->fullpath;
        pair<set<string>::iterator,bool> setret;
        setret = dimnamelist.insert((var->dims)[0]->name);
        if (true == setret.second) 
            Insert_One_NameSizeMap_Element((var->dims)[0]->name,(var->dims)[0]->size);
    }

    // No dimension, add fake dim names, this will rarely happen.
    else {

        set<hsize_t> fakedimsize;
        pair<set<hsize_t>::iterator,bool> setsizeret;
        for (vector<Dimension *>::iterator ird= var->dims.begin();
            ird != var->dims.end(); ++ird) {
                Add_One_FakeDim_Name(*ird);
                setsizeret = fakedimsize.insert((*ird)->size);
                // Avoid the same size dimension sharing the same dimension name.
                if (false == setsizeret.second)   
                    Adjust_Duplicate_FakeDim_Name(*ird);
        }
    }

}

void GMFile::Add_UseDimscale_Var_Dim_Names_General_Product(Var *var,Attribute*dimlistattr) 
throw (Exception){
    
    ssize_t objnamelen = -1;
    hobj_ref_t rbuf;
    //hvl_t *vlbuf = NULL;
    vector<hvl_t> vlbuf;
    
    hid_t dset_id = -1;
    hid_t attr_id = -1;
    hid_t atype_id = -1;
    hid_t amemtype_id = -1;
    hid_t aspace_id = -1;
    hid_t ref_dset = -1;


    if(NULL == dimlistattr) 
        throw2("Cannot obtain the dimension list attribute for variable ",var->name);

    if (0==var->rank) 
        throw2("The number of dimension should NOT be 0 for the variable ",var->name);
    
    try {

        //vlbuf = new hvl_t[var->rank];
        vlbuf.resize(var->rank);
    
        hid_t dset_id = H5Dopen(this->fileid,(var->fullpath).c_str(),H5P_DEFAULT);
        if (dset_id < 0) 
            throw2("Cannot open the dataset ",var->fullpath);

        attr_id = H5Aopen(dset_id,(dimlistattr->name).c_str(),H5P_DEFAULT);
        if (attr_id <0 ) 
            throw4("Cannot open the attribute ",dimlistattr->name," of HDF5 dataset ",var->fullpath);

        atype_id = H5Aget_type(attr_id);
        if (atype_id <0) 
            throw4("Cannot obtain the datatype of the attribute ",dimlistattr->name," of HDF5 dataset ",var->fullpath);

        amemtype_id = H5Tget_native_type(atype_id, H5T_DIR_ASCEND);

        if (amemtype_id < 0) 
            throw2("Cannot obtain the memory datatype for the attribute ",dimlistattr->name);


        if (H5Aread(attr_id,amemtype_id,&vlbuf[0]) <0)  
            throw2("Cannot obtain the referenced object for the variable ",var->name);
        

        vector<char> objname;
        int vlbuf_index = 0;

        // The dimension names of variables will be the HDF5 dataset names dereferenced from the DIMENSION_LIST attribute.
        for (vector<Dimension *>::iterator ird = var->dims.begin();
                ird != var->dims.end(); ++ird) {

            rbuf =((hobj_ref_t*)vlbuf[vlbuf_index].p)[0];
            if ((ref_dset = H5Rdereference(attr_id, H5R_OBJECT, &rbuf)) < 0) 
                throw2("Cannot dereference from the DIMENSION_LIST attribute  for the variable ",var->name);

            if ((objnamelen= H5Iget_name(ref_dset,NULL,0))<=0) 
                throw2("Cannot obtain the dataset name dereferenced from the DIMENSION_LIST attribute  for the variable ",var->name);
            objname.resize(objnamelen+1);
            if ((objnamelen= H5Iget_name(ref_dset,&objname[0],objnamelen+1))<=0) 
                throw2("Cannot obtain the dataset name dereferenced from the DIMENSION_LIST attribute  for the variable ",var->name);

            string objname_str = string(objname.begin(),objname.end());
            string trim_objname = objname_str.substr(0,objnamelen);
            (*ird)->name = string(trim_objname.begin(),trim_objname.end());

            pair<set<string>::iterator,bool> setret;
            setret = dimnamelist.insert((*ird)->name);
            if (true == setret.second) 
               Insert_One_NameSizeMap_Element((*ird)->name,(*ird)->size);
            (*ird)->newname = (*ird)->name;
            H5Dclose(ref_dset);
            ref_dset = -1;
            objname.clear();
            vlbuf_index++;
        }// for (vector<Dimension *>::iterator ird = var->dims.begin()
        if(vlbuf.size()!= 0) {

            if ((aspace_id = H5Aget_space(attr_id)) < 0)
                throw2("Cannot get hdf5 dataspace id for the attribute ",dimlistattr->name);

            if (H5Dvlen_reclaim(amemtype_id,aspace_id,H5P_DEFAULT,(void*)&vlbuf[0])<0) 
                throw2("Cannot successfully clean up the variable length memory for the variable ",var->name);

            H5Sclose(aspace_id);
           
        }

        H5Tclose(atype_id);
        H5Tclose(amemtype_id);
        H5Aclose(attr_id);
        H5Dclose(dset_id);
    
       // if(vlbuf != NULL)
        //  delete[] vlbuf;
    }

    catch(...) {

        if(atype_id != -1)
            H5Tclose(atype_id);

        if(amemtype_id != -1)
            H5Tclose(amemtype_id);

        if(aspace_id != -1)
            H5Sclose(aspace_id);

        if(attr_id != -1)
            H5Aclose(attr_id);

        if(dset_id != -1)
            H5Dclose(dset_id);

        //if(vlbuf != NULL)
         //   delete[] vlbuf;

        //throw1("Error in method GMFile::Add_UseDimscale_Var_Dim_Names_Mea_SeaWiFS_Ozone"); 
        throw;
    }
 
}


void GMFile::Handle_CVar() throw(Exception){

    // No support for coordinate variables for general HDF5 products
    // and ACOS_L2S
    if (General_Product == this->product_type ||
        ACOS_L2S == this->product_type) {
        if (GENERAL_DIMSCALE == this->gproduct_pattern)
            Handle_CVar_Dimscale_General_Product();
        else
            return;
    } 

    else if (Mea_SeaWiFS_L2 == this->product_type ||
        Mea_SeaWiFS_L3 == this->product_type) 
        Handle_CVar_Mea_SeaWiFS();

    else if (Aqu_L3 == this->product_type) 
        Handle_CVar_Aqu_L3(); 
    else if (OBPG_L3 == this->product_type)
        Handle_CVar_OBPG_L3();
    else if (SMAP == this->product_type) 
        Handle_CVar_SMAP();
    else if (Mea_Ozone == this->product_type) 
        Handle_CVar_Mea_Ozone();
    else if (GPMS_L3 == this->product_type || GPMM_L3 == this->product_type) 
        Handle_CVar_GPM_L3();
    else if (GPM_L1 == this->product_type)
        Handle_CVar_GPM_L1();
}

void GMFile::Handle_CVar_GPM_L1() throw(Exception) {

#if 0
    // Loop through the variable list to build the coordinates.
    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {
        if((*irv)->name=="AlgorithmRuntimeInfo") {
            delete(*irv);
            this->vars.erase(irv); 
            break;
        }
    }
#endif
 
    // Loop through all variables to check 2-D "Latitude" and "Longitude". 
    // Create coordinate variables based on 2-D "Latitude" and  "Longitude".
    // Latitude[Xdim][YDim] Longitude[Xdim][YDim], Latitude <->Xdim, Longitude <->YDim.
    // Make sure to build cf dimension names cfdimname = latpath+ the lat dimension name.
    // We want to save dimension names of Latitude and Longitude since 
    // the Fake coordinate variables of these two dimensions should not be generated.
    // So we need to remember these dimension names.
    string ll_dim0,ll_dim1;
    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {
        if((*irv)->rank == 2 && (*irv)->name == "Latitude") {
            GMCVar* GMcvar = new GMCVar(*irv);
            size_t lat_pos = (*irv)->fullpath.rfind("Latitude");
            string lat_path = (*irv)->fullpath.substr(0,lat_pos);
            GMcvar->cfdimname = lat_path + ((*irv)->dims)[0]->name;    
            ll_dim0 = ((*irv)->dims)[0]->name;
            GMcvar->cvartype = CV_EXIST;
            GMcvar->product_type = product_type;
            this->cvars.push_back(GMcvar);
            delete(*irv);
            this->vars.erase(irv);
            irv--;
        }
        if((*irv)->rank == 2 && (*irv)->name == "Longitude") {
            GMCVar* GMcvar = new GMCVar(*irv);
            size_t lon_pos = (*irv)->fullpath.rfind("Longitude");
            string lon_path = (*irv)->fullpath.substr(0,lon_pos);
            GMcvar->cfdimname = lon_path + ((*irv)->dims)[1]->name;    
            ll_dim1 = ((*irv)->dims)[1]->name;
            GMcvar->cvartype = CV_EXIST;
            GMcvar->product_type = product_type;
            this->cvars.push_back(GMcvar);
            delete(*irv);
            this->vars.erase(irv);
            irv--;
        }
    }// for (vector<Var *>::iterator irv = this->vars.begin();...

#if 0
    // Loop through all variables and create a dim set.
    set<string> cvdimset;
    pair<set<string>::iterator,bool> setret;
    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {
        for(vector<Dimension *>::iterator ird = (*irv)->dims.begin();
            ird != (*irv)->dims.end(); ++ird) {
            setret = cvdimset.insert((*ird)->name);
cerr<<"var name is "<<(*irv)->fullpath <<endl;
            if (true == setret.second) {
cerr<<"dim name is "<<(*ird)->name <<endl;
                Insert_One_NameSizeMap_Element((*ird)->name,(*ird)->size);
            }
        }
    }// for (vector<Var *>::iterator irv = this->vars.begin();...
#endif

    // For each dimension, create a coordinate variable.
    // Here we just need to loop through the map dimname_to_dimsize,
    // use the name and the size to create coordinate variables.
    for (map<string,hsize_t>::const_iterator itd = dimname_to_dimsize.begin();
                                            itd!=dimname_to_dimsize.end();++itd) {
        if((itd->first) != ll_dim0 && (itd->first) != ll_dim1) {
            GMCVar*GMcvar = new GMCVar();
            Create_Missing_CV(GMcvar,itd->first);
            this->cvars.push_back(GMcvar);
        }
    }//for (map<string,hsize_t>::iterator itd = dimname_to_dimsize.begin(); ...

    

}

void GMFile::Handle_CVar_GPM_L3() throw(Exception){

    iscoard = true;
    //map<string,hsize_t>::iterator itd;
    
    // Here we just need to loop through the map dimname_to_dimsize,
    // use the name and the size to create coordinate variables.
    for (map<string,hsize_t>::const_iterator itd = dimname_to_dimsize.begin();
                                            itd!=dimname_to_dimsize.end();++itd) {

        GMCVar*GMcvar = new GMCVar();
        if("nlon" == itd->first || "nlat" == itd->first
           || "lnH" == itd->first || "ltH" == itd->first
           || "lnL" == itd->first || "ltL" == itd->first) {
            GMcvar->name = itd->first;
            GMcvar->newname = GMcvar->name;
            GMcvar->fullpath = GMcvar->name;
            GMcvar->rank = 1;
            GMcvar->dtype = H5FLOAT32; 
            Dimension* gmcvar_dim = new Dimension(itd->second);
            gmcvar_dim->name = GMcvar->name;
            gmcvar_dim->newname = gmcvar_dim->name;
            GMcvar->dims.push_back(gmcvar_dim); 
            GMcvar->cfdimname = gmcvar_dim->name;
            if ("nlat" ==GMcvar->name || "ltH" == GMcvar->name 
                 || "ltL" == GMcvar->name) 
                GMcvar->cvartype = CV_LAT_MISS;
            else if ("nlon" == GMcvar->name || "lnH" == GMcvar->name
                 || "lnL" == GMcvar->name) 
                GMcvar->cvartype = CV_LON_MISS;
            GMcvar->product_type = product_type;
        }   
        else if (("nlayer" == itd->first && 28 == itd->second) ||
                 ("hgt" == itd->first && 5 == itd->second) ||
                 ("nalt" == itd->first && 5 == itd->second)){
            GMcvar->name = itd->first;
            GMcvar->newname = GMcvar->name;
            GMcvar->fullpath = GMcvar->name;
            GMcvar->rank = 1;
            GMcvar->dtype = H5FLOAT32;
            Dimension* gmcvar_dim = new Dimension(itd->second);
            gmcvar_dim->name = GMcvar->name;
            gmcvar_dim->newname = gmcvar_dim->name;
            GMcvar->dims.push_back(gmcvar_dim);
            GMcvar->cfdimname = gmcvar_dim->name;
            GMcvar->cvartype  = CV_SPECIAL;
            GMcvar->product_type = product_type;
        }
        else 
            Create_Missing_CV(GMcvar,itd->first);
        this->cvars.push_back(GMcvar);
    }//for (map<string,hsize_t>::iterator itd = dimname_to_dimsize.begin(); ...

}

void GMFile::Handle_CVar_Mea_SeaWiFS() throw(Exception){

    pair<set<string>::iterator,bool> setret;
    set<string>tempdimnamelist = dimnamelist;

    for (set<string>::iterator irs = dimnamelist.begin();
            irs != dimnamelist.end();++irs) {
        for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {
            if ((*irs)== (*irv)->fullpath) {

                if (!iscoard && (("/natrack" == (*irs)) 
                                 || "/nxtrack" == (*irs)))
                    continue;

                if((*irv)->dims.size()!=1) 
                    throw3("Coard coordinate variable",(*irv)->name, "is not 1D");
                   // Create Coordinate variables.
                   tempdimnamelist.erase(*irs);
                   GMCVar* GMcvar = new GMCVar(*irv);
                   GMcvar->cfdimname = *irs;
                   GMcvar->cvartype = CV_EXIST;
                   GMcvar->product_type = product_type;
                   this->cvars.push_back(GMcvar); 
                   delete(*irv);
                   this->vars.erase(irv);
                   irv--;
            } // if ((*irs)== (*irv)->fullpath)
            else if(false == iscoard) { 
            // 2-D lat/lon, natrack maps to lat, nxtrack maps to lon.
                 
                if ((((*irs) =="/natrack") && ((*irv)->fullpath == "/latitude"))
                  ||(((*irs) =="/nxtrack") && ((*irv)->fullpath == "/longitude"))) {
                    tempdimnamelist.erase(*irs);
                    GMCVar* GMcvar = new GMCVar(*irv);
                    GMcvar->cfdimname = *irs;    
                    GMcvar->cvartype = CV_EXIST;
                    GMcvar->product_type = product_type;
                    this->cvars.push_back(GMcvar);
                    delete(*irv);
                    this->vars.erase(irv);
                    irv--;
                }
            }// else if(false == iscoard)
        } // for (vector<Var *>::iterator irv = this->vars.begin() ... 
    } // for (set<string>::iterator irs = dimnamelist.begin() ...

    // Creating the missing "third-dimension" according to the dimension names.
    // This may never happen for the current MeaSure SeaWiFS, but put it here for code coherence and completeness.
    // KY 12-30-2011
    for (set<string>::iterator irs = tempdimnamelist.begin();
        irs != tempdimnamelist.end();++irs) {
        GMCVar*GMcvar = new GMCVar();
        Create_Missing_CV(GMcvar,*irs);
        this->cvars.push_back(GMcvar);
    }
}

void GMFile::Handle_CVar_SMAP() throw(Exception) {

    pair<set<string>::iterator,bool> setret;
    set<string>tempdimnamelist = dimnamelist;
    string tempvarname;
    string key0 = "_lat";
    string key1 = "_lon";
    string smapdim0 ="YDim";
    string smapdim1 ="XDim";

    bool foundkey0 = false;
    bool foundkey1 = false;

    set<string> itset;

    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {

        tempvarname = (*irv)->name;

        if ((tempvarname.size() > key0.size())&&
                (key0 == tempvarname.substr(tempvarname.size()-key0.size(),key0.size()))){
                foundkey0 = true;
            if (dimnamelist.find(smapdim0)== dimnamelist.end()) 
                throw5("variable ",tempvarname," must have dimension ",smapdim0," , but not found ");

                tempdimnamelist.erase(smapdim0);
                GMCVar* GMcvar = new GMCVar(*irv);
                GMcvar->newname = GMcvar->name; // Remove the path, just use the variable name
                GMcvar->cfdimname = smapdim0;    
                GMcvar->cvartype = CV_EXIST;
                GMcvar->product_type = product_type;
                this->cvars.push_back(GMcvar);
                delete(*irv);
                this->vars.erase(irv);
                irv--;
        }// if ((tempvarname.size() > key0.size())&& ...
                    
        else if ((tempvarname.size() > key1.size())&& 
                (key1 == tempvarname.substr(tempvarname.size()-key1.size(),key1.size()))){
                foundkey1 = true;
            if (dimnamelist.find(smapdim1)== dimnamelist.end()) 
                throw5("variable ",tempvarname," must have dimension ",smapdim1," , but not found ");

                tempdimnamelist.erase(smapdim1);

                GMCVar* GMcvar = new GMCVar(*irv);
                GMcvar->newname = GMcvar->name;
                GMcvar->cfdimname = smapdim1;    
                GMcvar->cvartype = CV_EXIST;
                GMcvar->product_type = product_type;
                this->cvars.push_back(GMcvar);
                delete(*irv);
                this->vars.erase(irv);
                irv--;
        }// else if ((tempvarname.size() > key1.size())&& ...
        if (true == foundkey0 && true == foundkey1) 
            break;
            
    } // for (vector<Var *>::iterator irv = this->vars.begin(); ...

    for (set<string>::iterator irs = tempdimnamelist.begin();
        irs != tempdimnamelist.end();++irs) {

        GMCVar*GMcvar = new GMCVar();
        Create_Missing_CV(GMcvar,*irs);
        this->cvars.push_back(GMcvar);
    }

}

void GMFile::Handle_CVar_Aqu_L3() throw(Exception) {

    iscoard = true;
    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {

        if ( "l3m_data" == (*irv)->name) { 
            for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                        ird != (*irv)->dims.end(); ++ird) {
                GMCVar*GMcvar = new GMCVar();
                GMcvar->name = (*ird)->name;
                GMcvar->newname = GMcvar->name;
                GMcvar->fullpath = GMcvar->name;
                GMcvar->rank = 1;
                GMcvar->dtype = H5FLOAT32; 
                Dimension* gmcvar_dim = new Dimension((*ird)->size);
                gmcvar_dim->name = GMcvar->name;
                gmcvar_dim->newname = gmcvar_dim->name;
                GMcvar->dims.push_back(gmcvar_dim); 
                GMcvar->cfdimname = gmcvar_dim->name;
                if ("lat" ==GMcvar->name ) GMcvar->cvartype = CV_LAT_MISS;
                if ("lon" == GMcvar->name ) GMcvar->cvartype = CV_LON_MISS;
                GMcvar->product_type = product_type;
                this->cvars.push_back(GMcvar);
            } // for (vector<Dimension *>::iterator ird = (*irv)->dims.begin(); ...
        } // if ( "l3m_data" == (*irv)->name) 
    }//for (vector<Var *>::iterator irv = this->vars.begin(); ...
 
}

void GMFile::Handle_CVar_Mea_Ozone() throw(Exception){

    pair<set<string>::iterator,bool> setret;
    set<string>tempdimnamelist = dimnamelist;

    if(false == iscoard) 
        throw1("Measure Ozone level 3 zonal average product must follow COARDS conventions");

    for (set<string>::iterator irs = dimnamelist.begin();
            irs != dimnamelist.end();++irs) {
        for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {
            if ((*irs)== (*irv)->fullpath) {

                if((*irv)->dims.size()!=1) 
                    throw3("Coard coordinate variable",(*irv)->name, "is not 1D");

                   // Create Coordinate variables.
                   tempdimnamelist.erase(*irs);
                   GMCVar* GMcvar = new GMCVar(*irv);
                   GMcvar->cfdimname = *irs;
                   GMcvar->cvartype = CV_EXIST;
                   GMcvar->product_type = product_type;
                   this->cvars.push_back(GMcvar); 
                   delete(*irv);
                   this->vars.erase(irv);
                   irv--;
            } // if ((*irs)== (*irv)->fullpath)
       } // for (vector<Var *>::iterator irv = this->vars.begin();
    } // for (set<string>::iterator irs = dimnamelist.begin();

    // Wait for the final product to see if the following statement is true. Now comment out.
    //if(false == tempdimnamelist.empty()) throw1("Measure Ozone level 3 new data shouldnot have missing dimensions");
    for (set<string>::iterator irs = tempdimnamelist.begin();
        irs != tempdimnamelist.end();irs++) {

        GMCVar*GMcvar = new GMCVar();
        Create_Missing_CV(GMcvar,*irs);
        this->cvars.push_back(GMcvar);
    }
}


void GMFile::Handle_CVar_Dimscale_General_Product() throw(Exception) {
    pair<set<string>::iterator,bool> setret;
    set<string>tempdimnamelist = dimnamelist;

    if(false == iscoard) 
        throw1("Currently products that use HDF5 dimension scales  must follow COARDS conventions");

    for (set<string>::iterator irs = dimnamelist.begin();
            irs != dimnamelist.end();++irs) {
        for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {

            // This is the dimension scale dataset; it should be changed to a coordinate variable.
            if ((*irs)== (*irv)->fullpath) {

                if((*irv)->dims.size()!=1) 
                    throw3("COARDS coordinate variable",(*irv)->name, "is not 1D");

                // Create Coordinate variables.
                tempdimnamelist.erase(*irs);
                GMCVar* GMcvar = new GMCVar(*irv);
                GMcvar->cfdimname = *irs;

                // Check if this is just a netCDF-4 dimension. 
                bool is_netcdf_dimension = Is_netCDF_Dimension(*irv);
                if (true == is_netcdf_dimension)
                   GMcvar->cvartype = CV_FILLINDEX;
                else 
                    GMcvar->cvartype = CV_EXIST;

                GMcvar->product_type = product_type;
                this->cvars.push_back(GMcvar); 
                delete(*irv);
                this->vars.erase(irv);
                irv--;
            } // if ((*irs)== (*irv)->fullpath)
       } // for (vector<Var *>::iterator irv = this->vars.begin();
    } // for (set<string>::iterator irs = dimnamelist.begin();

    // Wait for the final product to see if the following statement is true. Now comment out.
    //if(false == tempdimnamelist.empty()) throw1("Measure Ozone level 3 new data shouldnot have missing dimensions");
    for (set<string>::iterator irs = tempdimnamelist.begin();
        irs != tempdimnamelist.end();irs++) {

        GMCVar*GMcvar = new GMCVar();
        Create_Missing_CV(GMcvar,*irs);
        this->cvars.push_back(GMcvar);
    }

}

void GMFile::Handle_CVar_OBPG_L3() throw(Exception) {

    if (GENERAL_DIMSCALE == this->gproduct_pattern)
            Handle_CVar_Dimscale_General_Product();
    // Change the CV Type of the corresponding CVs of lat and lon from CV_FILLINDEX to CV_LATMISS or CV_LONMISS
    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {

        // Here I try to avoid using the dimension name row and column to find the lat/lon dimension size.
        // So I am looking for a 2-D floating-point array or a 2-D array under the group geophsical_data.
        // This may be subject to change if OBPG level 3 change its arrangement of variables.
        // KY 2014-09-29
 
        if((*irv)->rank == 2) {

            if(((*irv)->fullpath.find("/geophsical_data") == 0) || ((*irv)->dtype == H5FLOAT32)) {

                size_t lat_size = (*irv)->getDimensions()[0]->size;
                string lat_name = (*irv)->getDimensions()[0]->name;
                size_t lon_size = (*irv)->getDimensions()[1]->size;
                string lon_name = (*irv)->getDimensions()[1]->name;
                size_t temp_size = 0;
                string temp_name;
                H5DataType ll_dtype = (*irv)->dtype;

//cerr<<"lat_name is "<<lat_name <<endl;
//cerr<<"lon_name is "<<lon_name <<endl;
                // We always assume that longitude size is greater than latitude size.
                if(lat_size >lon_size) {
                    temp_size = lon_size;
                    temp_name = lon_name;
                    lon_size = lat_size;
                    lon_name = lat_name;
                    lat_size = temp_size;
                    lat_name = temp_name;
                }
                for (vector<GMCVar *>::iterator ircv = this->cvars.begin();
                    ircv != this->cvars.end(); ++ircv) {
                    if((*ircv)->cvartype == CV_FILLINDEX) {
                        if((*ircv)->getDimensions()[0]->size == lat_size &&
                           (*ircv)->getDimensions()[0]->name == lat_name) {
                            (*ircv)->cvartype = CV_LAT_MISS;
                            (*ircv)->dtype = ll_dtype;
                            for (vector<Attribute *>::iterator ira = (*ircv)->attrs.begin();
                                ira != (*ircv)->attrs.end(); ++ira) {
                                if ((*ira)->name == "NAME") {
                                    delete (*ira);
                                    (*ircv)->attrs.erase(ira);
                                    break;
                                }
                            }
                        }
                        else if((*ircv)->getDimensions()[0]->size == lon_size &&
                           (*ircv)->getDimensions()[0]->name == lon_name) {
                            (*ircv)->cvartype = CV_LON_MISS;
                            (*ircv)->dtype = ll_dtype;
                            for (vector<Attribute *>::iterator ira = (*ircv)->attrs.begin();
                                ira != (*ircv)->attrs.end(); ++ira) {
                                if ((*ira)->name == "NAME") {
                                    delete (*ira);
                                    (*ircv)->attrs.erase(ira);
                                    break;
                                }
                            }
                        }

                    }
                }
                break;

            } // if(((*irv)->fullpath.find("/geophsical_data") == 0) || ((*irv)->dtype == H5FLOAT32))
        } // if((*irv)->rank == 2)
    } // for (vector<Var *>::iterator irv = this->vars.begin();

}

void GMFile::Handle_SpVar() throw(Exception){
    if (ACOS_L2S == product_type) 
        Handle_SpVar_ACOS();
    else if(GPM_L1 == product_type) {
        // Loop through the variable list to build the coordinates.
        for (vector<Var *>::iterator irv = this->vars.begin();
                    irv != this->vars.end(); ++irv) {
            if((*irv)->name=="AlgorithmRuntimeInfo") {
                delete(*irv);
                this->vars.erase(irv); 
                break;
            }
        }
    }

    else if(GPMM_L3 == product_type || GPMS_L3 == product_type) {

        for (vector<Var *>::iterator irv = this->vars.begin();
                    irv != this->vars.end(); ++irv) {
            if((*irv)->name=="InputFileNames") {
                delete(*irv);
                this->vars.erase(irv);
                irv--;
            }
            else if((*irv)->name=="InputAlgorithmVersions") {
                delete(*irv);
                this->vars.erase(irv);
                irv--;
            }
            else if((*irv)->name=="InputGenerationDateTimes") {
                delete(*irv);
                this->vars.erase(irv);
                irv--;
            }

        }

   }

}

void GMFile::Handle_SpVar_ACOS() throw(Exception) {

    //The ACOS only have 64-bit variables. So we will not handle attributes yet.
    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {
        if (H5INT64 == (*irv)->getType()) {
            
            // First: Time Part of soundingid
            GMSPVar * spvar = new GMSPVar(*irv);
            spvar->name = (*irv)->name +"_Time";
            spvar->newname = (*irv)->newname+"_Time";
            spvar->dtype = H5INT32;
            spvar->otype = (*irv)->getType();
            spvar->sdbit = 1;

            // 2 digit hour, 2 digit min, 2 digit seconds
            spvar->numofdbits = 6;
            this->spvars.push_back(spvar);

            // Second: Date Part of soundingid
            GMSPVar * spvar2 = new GMSPVar(*irv);
            spvar2->name = (*irv)->name +"_Date";
            spvar2->newname = (*irv)->newname+"_Date";
            spvar2->dtype = H5INT32;
            spvar2->otype = (*irv)->getType();
            spvar2->sdbit = 7;

            // 4 digit year, 2 digit month, 2 digit day
            spvar2->numofdbits = 8;
            this->spvars.push_back(spvar2);

            delete(*irv);
            this->vars.erase(irv);
            irv--;
        } // if (H5INT64 == (*irv)->getType())
    } // for (vector<Var *>::iterator irv = this->vars.begin(); ...
}

            
void GMFile::Adjust_Obj_Name() throw(Exception) {

    if(Mea_Ozone == product_type) 
        Adjust_Mea_Ozone_Obj_Name();

    if(GPMS_L3 == product_type || GPMM_L3 == product_type)
        Adjust_GPM_L3_Obj_Name();

// Just for debugging
#if 0
for (vector<Var*>::iterator irv2 = this->vars.begin();
    irv2 != this->vars.end(); irv2++) {
    for (vector<Dimension *>::iterator ird = (*irv2)->dims.begin();
         ird !=(*irv2)->dims.end(); ird++) {
         cerr<<"Dimension name afet Adjust_Obj_Name "<<(*ird)->newname <<endl;
    }
}
#endif


}

void GMFile:: Adjust_GPM_L3_Obj_Name() throw(Exception) {

//cerr<<"number of group is "<<this->groups.size() <<endl;
    string objnewname;
    // In this definition, root group is not considered as a group.
    if(this->groups.size() <= 1) {
    for (vector<Var *>::iterator irv = this->vars.begin();
                   irv != this->vars.end(); ++irv) {
        objnewname =  HDF5CFUtil::obtain_string_after_lastslash((*irv)->newname);
        if (objnewname !="") 
           (*irv)->newname = objnewname;
    }
    }
    else {
        for (vector<Var *>::iterator irv = this->vars.begin();
                   irv != this->vars.end(); ++irv) {
//cerr<<"(*irv)->newname is "<<(*irv)->newname <<endl;
        size_t grid_group_path_pos = ((*irv)->newname.substr(1)).find_first_of("/");
        objnewname =  ((*irv)->newname).substr(grid_group_path_pos+2);
        (*irv)->newname = objnewname;
    }


    }
}

void GMFile:: Adjust_Mea_Ozone_Obj_Name() throw(Exception) {

    string objnewname;
    for (vector<Var *>::iterator irv = this->vars.begin();
                   irv != this->vars.end(); ++irv) {
        objnewname =  HDF5CFUtil::obtain_string_after_lastslash((*irv)->newname);
        if (objnewname !="") 
           (*irv)->newname = objnewname;

#if 0
//Just for debugging
for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                ird !=(*irv)->dims.end();++ird) {
 cerr<<"Ozone dim. name "<<(*ird)->name <<endl;
 cerr<<"Ozone dim. new name "<<(*ird)->newname <<endl;
}
#endif

    }

    for (vector<GMCVar *>::iterator irv = this->cvars.begin();
                irv != this->cvars.end(); ++irv) {
        objnewname =  HDF5CFUtil::obtain_string_after_lastslash((*irv)->newname);
        if (objnewname !="")
           (*irv)->newname = objnewname;
#if 0
 //Just for debugging
for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                ird !=(*irv)->dims.end();++ird) {
 cerr<<"Ozone CV dim. name "<<(*ird)->name <<endl;
 cerr<<"Ozone CV dim. new name "<<(*ird)->newname <<endl;
}   
#endif
    }
}
void GMFile::Flatten_Obj_Name(bool include_attr) throw(Exception){
     
    File::Flatten_Obj_Name(include_attr);

    for (vector<GMCVar *>::iterator irv = this->cvars.begin();
             irv != this->cvars.end(); ++irv) {
        (*irv)->newname = get_CF_string((*irv)->newname);

        for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                        ird != (*irv)->dims.end(); ++ird) { 
            (*ird)->newname = get_CF_string((*ird)->newname);
        }

        

        if (true == include_attr) {
            for (vector<Attribute *>::iterator ira = (*irv)->attrs.begin();
                        ira != (*irv)->attrs.end(); ++ira) 
                (*ira)->newname = get_CF_string((*ira)->newname);
                
        }

    }

    for (vector<GMSPVar *>::iterator irv = this->spvars.begin();
                irv != this->spvars.end(); ++irv) {
        (*irv)->newname = get_CF_string((*irv)->newname);

        for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                     ird != (*irv)->dims.end(); ++ird) 
            (*ird)->newname = get_CF_string((*ird)->newname);

        if (true == include_attr) {
            for (vector<Attribute *>::iterator ira = (*irv)->attrs.begin();
                        ira != (*irv)->attrs.end(); ++ira) 
                  (*ira)->newname = get_CF_string((*ira)->newname);
                
        }
    }

// Just for debugging
#if 0
for (vector<Var*>::iterator irv2 = this->vars.begin();
    irv2 != this->vars.end(); irv2++) {
    for (vector<Dimension *>::iterator ird = (*irv2)->dims.begin();
         ird !=(*irv2)->dims.end(); ird++) {
         cerr<<"Dimension name afet Flatten_Obj_Name "<<(*ird)->newname <<endl;
    }
}
#endif


}

void GMFile::Handle_Obj_NameClashing(bool include_attr) throw(Exception) {

    // objnameset will be filled with all object names that we are going to check the name clashing.
    // For example, we want to see if there are any name clashings for all variable names in this file.
    // objnameset will include all variable names. If a name clashing occurs, we can figure out from the set operation immediately.

    set<string>objnameset;
    Handle_GMCVar_NameClashing(objnameset);
    Handle_GMSPVar_NameClashing(objnameset);
    File::Handle_GeneralObj_NameClashing(include_attr,objnameset);
    if (true == include_attr) {
        Handle_GMCVar_AttrNameClashing();
        Handle_GMSPVar_AttrNameClashing();
    }
    // Moving to h5gmcfdap.cc, right after Adjust_Dim_Name
    //Handle_DimNameClashing();
}

void GMFile::Handle_GMCVar_NameClashing(set<string> &objnameset ) throw(Exception) {

    GMHandle_General_NameClashing(objnameset,this->cvars);
}

void GMFile::Handle_GMSPVar_NameClashing(set<string> &objnameset ) throw(Exception) {

    GMHandle_General_NameClashing(objnameset,this->spvars);
}


void GMFile::Handle_GMCVar_AttrNameClashing() throw(Exception) {

    set<string> objnameset;

    for (vector<GMCVar *>::iterator irv = this->cvars.begin();
        irv != this->cvars.end(); ++irv) {
        Handle_General_NameClashing(objnameset,(*irv)->attrs);
        objnameset.clear();
    }
}
void GMFile::Handle_GMSPVar_AttrNameClashing() throw(Exception) {

    set<string> objnameset;

    for (vector<GMSPVar *>::iterator irv = this->spvars.begin();
        irv != this->spvars.end(); ++irv) {
        Handle_General_NameClashing(objnameset,(*irv)->attrs);
        objnameset.clear();
    }
}

//class T must have member string newname
template<class T> void
GMFile::GMHandle_General_NameClashing(set <string>&objnameset, vector<T*>& objvec) throw(Exception){

    pair<set<string>::iterator,bool> setret;
    set<string>::iterator iss;

    vector<string> clashnamelist;
    vector<string>::iterator ivs;

    map<int,int> cl_to_ol;
    int ol_index = 0;
    int cl_index = 0;

    typename vector<T*>::iterator irv;
    //for (vector<T*>::iterator irv = objvec.begin();

    for (irv = objvec.begin();
                irv != objvec.end(); ++irv) {

        setret = objnameset.insert((*irv)->newname);
        if (false == setret.second ) {
            clashnamelist.insert(clashnamelist.end(),(*irv)->newname);
            cl_to_ol[cl_index] = ol_index;
            cl_index++;
        }
        ol_index++;
    }


    // Now change the clashed elements to unique elements; 
    // Generate the set which has the same size as the original vector.

    for (ivs=clashnamelist.begin(); ivs!=clashnamelist.end(); ++ivs) {
        int clash_index = 1;
        string temp_clashname = *ivs +'_';
        HDF5CFUtil::gen_unique_name(temp_clashname,objnameset,clash_index);
        *ivs = temp_clashname;
    }


    // Now go back to the original vector, make it unique.
    for (unsigned int i =0; i <clashnamelist.size(); i++)
        objvec[cl_to_ol[i]]->newname = clashnamelist[i];
     
}

void GMFile::Handle_DimNameClashing() throw(Exception){

//cerr<<"coming to DimNameClashing "<<endl;

    // ACOS L2S product doesn't need the dimension name clashing check based on our current understanding. KY 2012-5-16
    if (ACOS_L2S == product_type) return;

    map<string,string>dimname_to_dimnewname;
    pair<map<string,string>::iterator,bool>mapret;
    set<string> dimnameset;
    vector<Dimension*>vdims;
    set<string> dimnewnameset;
    pair<set<string>::iterator,bool> setret;

    // First: Generate the dimset/dimvar based on coordinate variables.
    for (vector<GMCVar *>::iterator irv = this->cvars.begin();
             irv !=this->cvars.end(); ++irv) {
        for (vector <Dimension *>:: iterator ird = (*irv)->dims.begin();
            ird !=(*irv)->dims.end();++ird) {
            //setret = dimnameset.insert((*ird)->newname);
            setret = dimnameset.insert((*ird)->name);
            if (true == setret.second) 
                vdims.push_back(*ird); 
        }
    }
    
    // For some cases, dimension names are provided but there are no corresponding coordinate
    // variables. For now, we will assume no such cases.
    // Actually, we find such a case in our fake testsuite. So we need to fix it.
    for(vector<Var *>::iterator irv= this->vars.begin();
        irv != this->vars.end();++irv) {
        for (vector <Dimension *>:: iterator ird = (*irv)->dims.begin();
            ird !=(*irv)->dims.end();++ird) {
            //setret = dimnameset.insert((*ird)->newname);
            setret = dimnameset.insert((*ird)->name);
            if (setret.second) vdims.push_back(*ird);
        }
    }

    // For some cases, dimension names are provided but there are no corresponding coordinate
    // variables. For now, we will assume no such cases.
    GMHandle_General_NameClashing(dimnewnameset,vdims);
   
    // Third: Make dimname_to_dimnewname map
    for (vector<Dimension*>::iterator ird = vdims.begin();ird!=vdims.end();++ird) {
        mapret = dimname_to_dimnewname.insert(pair<string,string>((*ird)->name,(*ird)->newname));
        if (false == mapret.second) 
            throw4("The dimension name ",(*ird)->name," should map to ",
                                      (*ird)->newname);
    }

    // Fourth: Change the original dimension new names to the unique dimension new names
    for (vector<GMCVar *>::iterator irv = this->cvars.begin();
        irv !=this->cvars.end(); ++irv)
        for (vector <Dimension *>:: iterator ird = (*irv)->dims.begin();
            ird!=(*irv)->dims.end();++ird) 
           (*ird)->newname = dimname_to_dimnewname[(*ird)->name];

    for (vector<Var *>::iterator irv = this->vars.begin();
         irv != this->vars.end(); ++irv)
        for (vector <Dimension *>:: iterator ird = (*irv)->dims.begin();
            ird !=(*irv)->dims.end();++ird) 
           (*ird)->newname = dimname_to_dimnewname[(*ird)->name];

}
    

void GMFile::Adjust_Dim_Name() throw(Exception){

#if 0
    // Just for debugging
for (vector<Var*>::iterator irv2 = this->vars.begin();
       irv2 != this->vars.end(); irv2++) {
    for (vector<Dimension *>::iterator ird = (*irv2)->dims.begin();
           ird !=(*irv2)->dims.end(); ird++) {
         cerr<<"Dimension new name "<<(*ird)->newname <<endl;
       }
}
#endif

    // Only need for COARD conventions.
    if( true == iscoard) {
        for (vector<GMCVar *>::iterator irv = this->cvars.begin();
             irv !=this->cvars.end(); ++irv) {
            if ((*irv)->dims.size()!=1) 
                throw3("Coard coordinate variable ",(*irv)->name, "is not 1D");
            if ((*irv)->newname != (((*irv)->dims)[0]->newname)) {
                ((*irv)->dims)[0]->newname = (*irv)->newname;

                // For all variables that have this dimension,the dimension newname should also change.
                for (vector<Var*>::iterator irv2 = this->vars.begin();
                    irv2 != this->vars.end(); ++irv2) {
                    for (vector<Dimension *>::iterator ird = (*irv2)->dims.begin();
                        ird !=(*irv2)->dims.end(); ++ird) {
                        // This is the key, the dimension name of this dimension 
                        // should be equal to the dimension name of the coordinate variable.
                        // Then the dimension name matches and the dimension name should be changed to
                        // the new dimension name.
                        if ((*ird)->name == ((*irv)->dims)[0]->name) 
                            (*ird)->newname = ((*irv)->dims)[0]->newname;
                    }
                }
            } // if ((*irv)->newname != (((*irv)->dims)[0]->newname))
        }// for (vector<GMCVar *>::iterator irv = this->cvars.begin(); ...
   } // if( true == iscoard) 

// Just for debugging
#if 0
for (vector<Var*>::iterator irv2 = this->vars.begin();
    irv2 != this->vars.end(); irv2++) {
    for (vector<Dimension *>::iterator ird = (*irv2)->dims.begin();
         ird !=(*irv2)->dims.end(); ird++) {
         cerr<<"Dimension name afet Adjust_Dim_Name "<<(*ird)->newname <<endl;
    }
}
#endif


}

void 
GMFile:: Add_Supplement_Attrs(bool add_path) throw(Exception) {

    if (General_Product == product_type || true == add_path) {
        File::Add_Supplement_Attrs(add_path);   

         // Adding variable original name(origname) and full path(fullpath)
        for (vector<GMCVar *>::iterator irv = this->cvars.begin();
                irv != this->cvars.end(); ++irv) {
            if (((*irv)->cvartype == CV_EXIST) || ((*irv)->cvartype == CV_MODIFY)) {
                Attribute * attr = new Attribute();
                const string varname = (*irv)->name;
                const string attrname = "origname";
                Add_Str_Attr(attr,attrname,varname);
                (*irv)->attrs.push_back(attr);
            }
        }

        for (vector<GMCVar *>::iterator irv = this->cvars.begin();
                irv != this->cvars.end(); ++irv) {
            if (((*irv)->cvartype == CV_EXIST) || ((*irv)->cvartype == CV_MODIFY)) {
                Attribute * attr = new Attribute();
                const string varname = (*irv)->fullpath;
                const string attrname = "fullnamepath";
                Add_Str_Attr(attr,attrname,varname);
                (*irv)->attrs.push_back(attr);
            }
        }

        for (vector<GMSPVar *>::iterator irv = this->spvars.begin();
                irv != this->spvars.end(); ++irv) {
            Attribute * attr = new Attribute();
            const string varname = (*irv)->name;
            const string attrname = "origname";
            Add_Str_Attr(attr,attrname,varname);
            (*irv)->attrs.push_back(attr);
        }

        for (vector<GMSPVar *>::iterator irv = this->spvars.begin();
                irv != this->spvars.end(); ++irv) {
            Attribute * attr = new Attribute();
            const string varname = (*irv)->fullpath;
            const string attrname = "fullnamepath";
            Add_Str_Attr(attr,attrname,varname);
            (*irv)->attrs.push_back(attr);
        }
    } // if (General_Product == product_type || true == add_path)

    if(GPM_L1 == product_type || GPMS_L3 == product_type || GPMM_L3 == product_type)
        Add_GPM_Attrs();
    else if (Aqu_L3 == product_type) 
        Add_Aqu_Attrs();
    else if (Mea_SeaWiFS_L2 == product_type || Mea_SeaWiFS_L3 == product_type) 
        Add_SeaWiFS_Attrs();
        
}

void 
GMFile:: Add_GPM_Attrs() throw(Exception) {

    vector<HDF5CF::Var *>::const_iterator it_v;
    vector<HDF5CF::Attribute *>::const_iterator ira;
    const string attr_name_be_replaced = "CodeMissingValue";
    const string attr_new_name = "_FillValue";
    const string attr_cor_fill_value = "-9999.9";
    const string attr2_name_be_replaced = "Units";
    const string attr2_new_name ="units";

    // Need to convert String type CodeMissingValue to the corresponding _FilLValue
    // Create a function at HDF5CF.cc. use strtod,strtof,strtol etc. function to convert
    // string to the corresponding type.
    for (it_v = vars.begin(); it_v != vars.end(); ++it_v) {
        for(ira = (*it_v)->attrs.begin(); ira!= (*it_v)->attrs.end();ira++) {
            if((attr_name_be_replaced == (*ira)->name)) { 
                if((*ira)->dtype == H5FSTRING) 
                    Change_Attr_One_Str_to_Others((*ira),(*it_v));
                (*ira)->name = attr_new_name;
                (*ira)->newname = attr_new_name;
            }
        }

    }
     
    
    for (vector<GMCVar *>::iterator irv = this->cvars.begin();
                irv != this->cvars.end(); ++irv) {

        for(ira = (*irv)->attrs.begin(); ira!= (*irv)->attrs.end();ira++) {

            if(attr_name_be_replaced == (*ira)->name) {
                if((*ira)->dtype == H5FSTRING) 
                    Change_Attr_One_Str_to_Others((*ira),(*irv));
                (*ira)->name = attr_new_name;
                (*ira)->newname = attr_new_name;
                break;
            }
        }
    
      if(product_type == GPM_L1) {
        if ((*irv)->cvartype == CV_EXIST) {

            for(ira = (*irv)->attrs.begin(); ira!= (*irv)->attrs.end();ira++) {

                if(attr2_name_be_replaced == (*ira)->name) {

                    if((*irv)->name.find("Latitude") !=string::npos) {

                        string unit_value = "degrees_north";
                        (*ira)->value.clear();
                        Add_Str_Attr(*ira,attr2_new_name,unit_value);
                    //(*ira)->value.resize(unit_value.size());
                    //copy(unit_value.begin(),unit_value.end(),(*ira)->value.begin());
                    }

                    else if((*irv)->name.find("Longitude") !=string::npos) {

                        string unit_value = "degrees_east";
                        (*ira)->value.clear();
                        Add_Str_Attr(*ira,attr2_new_name,unit_value);
                    //(*ira)->value.resize(unit_value.size());
                    //copy(unit_value.begin(),unit_value.end(),(*ira)->value.begin());
                    }
                }
            } 
        }

        else if ((*irv)->cvartype == CV_NONLATLON_MISS) {
            
            string comment;
            const string attrname = "comment";
            Attribute*attr = new Attribute();

            {
            if((*irv)->name == "nchannel1") 
                comment = "Number of Swath S1 channels (10V 10H 19V 19H 23V 37V 37H 89V 89H).";
            else if((*irv)->name == "nchannel2") 
                comment = "Number of Swath S2 channels (166V 166H 183+/-3V 183+/-8V).";
            else if((*irv)->name == "nchan1")
                comment = "Number of channels in Swath 1.";
            else if((*irv)->name == "nchan2")
                comment = "Number of channels in Swath 2.";
            else if((*irv)->name == "VH") 
                comment = "Number of polarizations.";
            else if((*irv)->name == "GMIxyz") 
                comment = "x, y, z components in GMI instrument coordinate system.";
            else if((*irv)->name == "LNL")
                comment = "Linear and non-linear.";
            else if((*irv)->name == "nscan")
                comment = "Number of scans in the granule.";
            else if((*irv)->name == "nscan1")
                comment = "Typical number of Swath S1 scans in the granule.";
            else if((*irv)->name == "nscan2")
                comment = "Typical number of Swath S2 scans in the granule.";
            else if((*irv)->name == "npixelev")
                comment = "Number of earth view pixels in one scan.";
            else if((*irv)->name == "npixelht")
                comment = "Number of hot load pixels in one scan.";
            else if((*irv)->name == "npixelcs")
                comment = "Number of cold sky pixels in one scan.";
            else if((*irv)->name == "npixelfr")
                comment = "Number of full rotation earth view pixels in one scan.";
            else if((*irv)->name == "nfreq1")
                comment = "Number of frequencies in Swath 1.";
            else if((*irv)->name == "nfreq2")
                comment = "Number of frequencies in Swath 2.";
            else if((*irv)->name == "npix1")
                comment = "Number of pixels in Swath 1.";
            else if((*irv)->name == "npix2")
                comment = "Number of pixels in Swath 2.";
            else if((*irv)->name == "npix3")
                comment = "Number of pixels in Swath 3.";
            else if((*irv)->name == "npix4")
                comment = "Number of pixels in Swath 4.";
            else if((*irv)->name == "ncolds1")
                comment = "Maximum number of cold samples in Swath 1.";
            else if((*irv)->name == "ncolds2")
                comment = "Maximum number of cold samples in Swath 2.";
            else if((*irv)->name == "nhots1")
                comment = "Maximum number of hot samples in Swath 1.";
            else if((*irv)->name == "nhots2")
                comment = "Maximum number of hot samples in Swath 2.";
            else if((*irv)->name == "ntherm")
                comment = "Number of hot load thermisters.";
            else if((*irv)->name == "ntach")
                comment = "Number of tachometer readings.";
            else if((*irv)->name == "nsamt"){
                comment = "Number of sample types. ";
                comment = +"The types are: total science GSDR, earthview,hot load, cold sky.";
            }
            else if((*irv)->name == "nndiode")
                comment = "Number of noise diodes.";
            else if((*irv)->name == "n7")
                comment = "Number seven.";
            else if((*irv)->name == "nray")
                comment = "Number of angle bins in each NS scan."; 
            else if((*irv)->name == "nrayMS")
                comment = "Number of angle bins in each MS scan."; 
            else if((*irv)->name == "nrayHS")
                comment = "Number of angle bins in each HS scan."; 
            else if((*irv)->name == "nbin")
                comment = "Number of range bins in each NS and MS ray. Bin interval is 125m."; 
            else if((*irv)->name == "nbinHS")
                comment = "Number of range bins in each HS ray. Bin interval is 250m."; 
            else if((*irv)->name == "nbinSZP")
                comment = "Number of range bins for sigmaZeroProfile."; 
            else if((*irv)->name == "nbinSZPHS")
                comment = "Number of range bins for sigmaZeroProfile in each HS scan."; 
            else if((*irv)->name == "nNP")
                comment = "Number of NP kinds."; 
            else if((*irv)->name == "nearFar")
                comment = "Near reference, Far reference."; 
            else if((*irv)->name == "foreBack")
                comment = "Forward, Backward."; 
            else if((*irv)->name == "method")
                comment = "Number of SRT methods."; 
             else if((*irv)->name == "nNode")
                comment = "Number of binNode."; 
              else if((*irv)->name == "nDSD")
                comment = "Number of DSD parameters. Parameters are N0 and D0"; 
           else if((*irv)->name == "LS")
                comment = "Liquid, solid."; 
            }

            if(""==comment)
                delete attr;
            else {
                Add_Str_Attr(attr,attrname,comment);
                (*irv)->attrs.push_back(attr);
            }

        }
      }

      if(product_type == GPMS_L3 || product_type == GPMM_L3) {
        if ((*irv)->cvartype == CV_NONLATLON_MISS) {
            
            string comment;
            const string attrname = "comment";
            Attribute*attr = new Attribute();

            {
            if((*irv)->name == "chn") 
                comment = "Number of channels:Ku,Ka,KaHS,DPR.";
            else if((*irv)->name == "inst") 
                comment = "Number of instruments:Ku,Ka,KaHS.";
            else if((*irv)->name == "tim")
                comment = "Number of hours(local time).";
            else if((*irv)->name == "ang"){
                comment = "Number of angles.The meaning of ang is different for each channel.";
                comment +=  
                "For Ku channel all indices are used with the meaning 0,1,2,..6 =angle bins 24,";
                comment +=
                "(20,28),(16,32),(12,36),(8,40),(3,44),and (0,48).";
                comment +=
                "For Ka channel 4 indices are used with the meaning 0,1,2,3 = angle bins 12,(8,16),";
                comment +=
                "(4,20),and (0,24). For KaHS channel 4 indices are used with the meaning 0,1,2,3 =";
                comment += "angle bins(11,2),(7,16),(3,20),and (0.23).";
    
            }
            else if((*irv)->name == "rt") 
                comment = "Number of rain types: stratiform, convective,all.";
            else if((*irv)->name == "st") 
                comment = "Number of surface types:ocean,land,all.";
            else if((*irv)->name == "bin"){
                comment = "Number of bins in histogram. The thresholds are different for different";
                comment +=" variables. see the file specification for this algorithm.";
            }
            else if((*irv)->name == "nvar") {
                comment = "Number of phase bins. Bins are counts of phase less than 100, ";
                comment +="counts of phase greater than or equal to 100 and less than 200, ";
                comment +="counts of phase greater than or equal to 200.";
            }
            else if((*irv)->name == "AD")
                comment = "Ascending or descending half of the orbit.";
            }

            if(""==comment)
                delete attr;
            else {
                Add_Str_Attr(attr,attrname,comment);
                (*irv)->attrs.push_back(attr);
            }

        }
      }


      if ((*irv)->cvartype == CV_SPECIAL) {
            if((*irv)->name == "nlayer" || (*irv)->name == "hgt"
               || (*irv)->name == "nalt") {
                Attribute*attr = new Attribute();
                string unit_value = "km";
                Add_Str_Attr(attr,attr2_new_name,unit_value);
                (*irv)->attrs.push_back(attr);

                Attribute*attr1 = new Attribute();
                string attr1_axis="axis";
                string attr1_value = "Z";
                Add_Str_Attr(attr1,attr1_axis,attr1_value);
                (*irv)->attrs.push_back(attr1);

                Attribute*attr2 = new Attribute();
                string attr2_positive="positive";
                string attr2_value = "up";
                Add_Str_Attr(attr2,attr2_positive,attr2_value);
                (*irv)->attrs.push_back(attr2);

            }
            if((*irv)->name == "hgt" || (*irv)->name == "nalt"){
                Attribute*attr1 = new Attribute();
                string comment ="Number of heights above the earth ellipsoid";
                Add_Str_Attr(attr1,"comment",comment);
                (*irv)->attrs.push_back(attr1);
            }

        }

    }



    
// Old code, leave it for the time being
#if 0
    const string fill_value_attr_name = "_FillValue";
    vector<HDF5CF::Var *>::const_iterator it_v;
    vector<HDF5CF::Attribute *>::const_iterator ira;

    for (it_v = vars.begin();
                it_v != vars.end(); ++it_v) {

        bool has_fillvalue = false;
        for(ira = (*it_v)->attrs.begin(); ira!= (*it_v)->attrs.end();ira++) {
            if (fill_value_attr_name == (*ira)->name){
                has_fillvalue = true;
                break;
            }

        }

        // Add the fill value
        if (has_fillvalue != true ) {
            
            if(H5FLOAT32 == (*it_v)->dtype) {
                Attribute* attr = new Attribute();
                    float _FillValue = -9999.9;
                    Add_One_Float_Attr(attr,fill_value_attr_name,_FillValue);
               (*it_v)->attrs.push_back(attr);
            }
        }
    }// for (it_v = vars.begin(); ...
#endif

}

void 
GMFile:: Add_Aqu_Attrs() throw(Exception) {

    vector<HDF5CF::Var *>::const_iterator it_v;
    vector<HDF5CF::Attribute *>::const_iterator ira;

    const string orig_longname_attr_name = "Parameter";
    const string longname_attr_name ="long_name";
    string longname_value;
    

    const string orig_units_attr_name = "Units";
    const string units_attr_name = "units";
    string units_value;
    
    //    const string orig_valid_min_attr_name = "Data Minimum";
    const string orig_valid_min_attr_name = "Data Minimum";
    const string valid_min_attr_name = "valid_min";
    float valid_min_value;

    const string orig_valid_max_attr_name = "Data Maximum";
    const string valid_max_attr_name = "valid_max";
    float valid_max_value;

    // The fill value is -32767.0. However, No _FillValue attribute is added.
    // So add it here. KY 2012-2-16
 
    const string fill_value_attr_name = "_FillValue";
    float _FillValue = -32767.0;

    

    for (ira = this->root_attrs.begin(); ira != this->root_attrs.end(); ++ira) {
        if (orig_longname_attr_name == (*ira)->name) {
            Retrieve_H5_Attr_Value(*ira,"/");
            longname_value.resize((*ira)->value.size());
            copy((*ira)->value.begin(),(*ira)->value.end(),longname_value.begin());

        }
        else if (orig_units_attr_name == (*ira)->name) {
            Retrieve_H5_Attr_Value(*ira,"/");
            units_value.resize((*ira)->value.size());
            copy((*ira)->value.begin(),(*ira)->value.end(),units_value.begin());

        }
        else if (orig_valid_min_attr_name == (*ira)->name) {
            Retrieve_H5_Attr_Value(*ira,"/");
            memcpy(&valid_min_value,(void*)(&((*ira)->value[0])),(*ira)->value.size());
        }

        else if (orig_valid_max_attr_name == (*ira)->name) {
            Retrieve_H5_Attr_Value(*ira,"/");
            memcpy(&valid_max_value,(void*)(&((*ira)->value[0])),(*ira)->value.size());
        }
        
    }// for (ira = this->root_attrs.begin(); ira != this->root_attrs.end(); ++ira)

    // Level 3 variable name is l3m_data
    for (it_v = vars.begin();
                it_v != vars.end(); ++it_v) {
        if ("l3m_data" == (*it_v)->name) {

            // 1. Add the long_name attribute
            Attribute * attr = new Attribute();
            Add_Str_Attr(attr,longname_attr_name,longname_value);
            (*it_v)->attrs.push_back(attr);

            // 2. Add the units attribute
            attr = new Attribute();
            Add_Str_Attr(attr,units_attr_name,units_value);
            (*it_v)->attrs.push_back(attr);

            // 3. Add the valid_min attribute
            attr = new Attribute();
            Add_One_Float_Attr(attr,valid_min_attr_name,valid_min_value);
            (*it_v)->attrs.push_back(attr);

            // 4. Add the valid_max attribute
            attr = new Attribute();
            Add_One_Float_Attr(attr,valid_max_attr_name,valid_max_value);
            (*it_v)->attrs.push_back(attr);

            // 5. Add the _FillValue attribute
            attr = new Attribute();
            Add_One_Float_Attr(attr,fill_value_attr_name,_FillValue);
            (*it_v)->attrs.push_back(attr);

            break;
        }
    } // for (it_v = vars.begin(); ...
}

void 
GMFile:: Add_SeaWiFS_Attrs() throw(Exception) {

    // The fill value is -999.0. However, No _FillValue attribute is added.
    // So add it here. KY 2012-2-16
    const string fill_value_attr_name = "_FillValue";
    float _FillValue = -999.0;
    const string valid_range_attr_name = "valid_range";
    vector<HDF5CF::Var *>::const_iterator it_v;
    vector<HDF5CF::Attribute *>::const_iterator ira;


    for (it_v = vars.begin();
                it_v != vars.end(); ++it_v) {
        if (H5FLOAT32 == (*it_v)->dtype) {
            bool has_fillvalue = false;
            bool has_validrange = false;
            for(ira = (*it_v)->attrs.begin(); ira!= (*it_v)->attrs.end();ira++) {
                if (fill_value_attr_name == (*ira)->name){
                    has_fillvalue = true;
                    break;
                }

                else if(valid_range_attr_name == (*ira)->name) {
                    has_validrange = true;
                    break;
                }

            }
            // Add the fill value
            if (has_fillvalue != true && has_validrange != true ) {
                Attribute* attr = new Attribute();
                Add_One_Float_Attr(attr,fill_value_attr_name,_FillValue);
                (*it_v)->attrs.push_back(attr);
            }
        }// if (H5FLOAT32 == (*it_v)->dtype)
    }// for (it_v = vars.begin(); ...
}

void GMFile:: Handle_Coor_Attr() {

    string co_attrname = "coordinates";
    string co_attrvalue="";
    string unit_attrname = "units";
    string nonll_unit_attrvalue ="level";
    string lat_unit_attrvalue ="degrees_north";
    string lon_unit_attrvalue ="degrees_east";

    for (vector<GMCVar *>::iterator ircv = this->cvars.begin();
        ircv != this->cvars.end(); ++ircv) {
//cerr<<"CV name is "<<(*ircv)->name << " cv type is "<<(*ircv)->cvartype <<endl;

        if ((*ircv)->cvartype == CV_NONLATLON_MISS) {
           Attribute * attr = new Attribute();
           Add_Str_Attr(attr,unit_attrname,nonll_unit_attrvalue);
           (*ircv)->attrs.push_back(attr);
        }

        else if ((*ircv)->cvartype == CV_LAT_MISS) {
//cerr<<"Should add new attribute "<<endl;
           Attribute * attr = new Attribute();
//           float temp = -999.9;
//           Add_One_Float_Attr(attr,unit_attrname,temp);
           Add_Str_Attr(attr,unit_attrname,lat_unit_attrvalue);
           (*ircv)->attrs.push_back(attr);
//cerr<<"After adding new attribute "<<endl;
        }

        else if ((*ircv)->cvartype == CV_LON_MISS) {
           Attribute * attr = new Attribute();
           Add_Str_Attr(attr,unit_attrname,lon_unit_attrvalue);
           (*ircv)->attrs.push_back(attr);
        }
    } // for (vector<GMCVar *>::iterator ircv = this->cvars.begin(); ...
   
    if(product_type == Mea_SeaWiFS_L2) 
        return; 

    else if(product_type == GPM_L1) {
        Handle_GPM_l1_Coor_Attr();
        return;
    }
    else if (true == iscoard) 
        return;
   
    for (vector<Var *>::iterator irv = this->vars.begin();
                irv != this->vars.end(); ++irv) {
        bool coor_attr_keep_exist = false;

        for (vector<Attribute *>:: iterator ira =(*irv)->attrs.begin();
            ira !=(*irv)->attrs.end();++ira) {
            if (((*ira)->newname == "coordinates")) {
                if (product_type == SMAP) {
                    coor_attr_keep_exist = true;
                    break;
                }
                else {
                    delete (*ira);
                    (*irv)->attrs.erase(ira);
                    ira --;
                }
            }
        }// for (vector<Attribute *>:: iterator ira =(*irv)->attrs.begin(); ...

        if (true == coor_attr_keep_exist) 
            continue;
                
        for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                ird != (*irv)->dims.end(); ++ ird) {
            for (vector<GMCVar *>::iterator ircv = this->cvars.begin();
                ircv != this->cvars.end(); ++ircv) {
                if ((*ird)->name == (*ircv)->cfdimname) 
                    co_attrvalue = (co_attrvalue.empty())
                                    ?(*ircv)->newname:co_attrvalue + " "+(*ircv)->newname;
            }
        }

        if (false == co_attrvalue.empty()) {
            Attribute * attr = new Attribute();
            Add_Str_Attr(attr,co_attrname,co_attrvalue);
            (*irv)->attrs.push_back(attr);
        }

        co_attrvalue.clear();
    } // for (vector<Var *>::iterator irv = this->vars.begin(); ...
}

void GMFile:: Handle_GPM_l1_Coor_Attr() throw(Exception){

    // Build a map from CFdimname to 2-D lat/lon variable name, should be something like: aa_list[cfdimname]=s1_latitude .
    // Loop all variables
    // Inner loop: for all dims of a var
    // if(dimname matches the dim(not cfdim) name of one of 2-D lat/lon,
    // check if the variable's full path contains the path of one of 2-D lat/lon,
    // yes, build its cfdimname = path+ dimname, check this cfdimname with the cfdimname of the corresponding 2-D lat/lon
    //      If matched, save this latitude variable name as one of the coordinate variable. 
    //      else this is a 3rd-cv, just use the dimension name(or the corresponding cv name maybe through a map).

    // Prepare 1) 2-D CVar(lat,lon) corresponding dimension name set.
    //         2) cfdim name to cvar name map(don't need to use a map, just a holder. It should be fine. 


    // "coordinates" attribute name and value.  We only need to provide this atttribute for variables that have 2-D lat/lon 
    string co_attrname = "coordinates";
    string co_attrvalue="";

    // 2-D cv dimname set.
    set<string> cvar_2d_dimset;

    pair<map<string,string>::iterator,bool>mapret;

    // Hold the mapping from cfdimname to 2-D cvar name. Something like nscan->lat, npixel->lon 
    map<string,string>cfdimname_to_cvar2dname;

    // Loop through cv variables to build 2-D cv dimname set and the mapping from cfdimname to 2-D cvar name.
    for (vector<GMCVar *>::iterator irv = this->cvars.begin();
                irv != this->cvars.end(); ++irv) {
        //This CVar must be 2-D array.
        if((*irv)->rank == 2) { 
//cerr<<"2-D cv name is "<<(*irv)->name <<endl;
//cerr<<"2-D cv new name is "<<(*irv)->newname <<endl;
//cerr<<"(*irv)->cfdimname is "<<(*irv)->cfdimname <<endl;
            for(vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                  ird != (*irv)->dims.end(); ++ird) {
                cvar_2d_dimset.insert((*ird)->name);
            }
            mapret = cfdimname_to_cvar2dname.insert(pair<string,string>((*irv)->cfdimname,(*irv)->newname));      
            if (false == mapret.second)
                throw4("The cf dimension name ",(*irv)->cfdimname," should map to 2-D coordinate variable",
                                      (*irv)->newname);
        }
    }


    // Loop through the variable list to build the coordinates.
    for (vector<Var *>::iterator irv = this->vars.begin();
        irv != this->vars.end(); ++irv) {

        // Only apply to >2D variables.
        if((*irv)->rank >=2) {

            // The variable dimension names must be found in the 2D cvar dim. nameset.
            // The flag must be at least 2.
            short have_2d_dimnames_flag = 0;
            for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                ird !=(*irv)->dims.end();++ird) {
                if (cvar_2d_dimset.find((*ird)->name)!=cvar_2d_dimset.end())    
                    have_2d_dimnames_flag++;
            }

            // Final candidates to have 2-D CVar coordinates. 
            if(have_2d_dimnames_flag >=2) {
//cerr<<"var name "<<(*irv)->name <<" has 2-D CVar coordinates "<<endl;

                // Obtain the variable path
                string var_path;
                if((*irv)->fullpath.size() > (*irv)->name.size()) 
                    var_path=(*irv)->fullpath.substr(0,(*irv)->fullpath.size()-(*irv)->name.size());
                else 
                    throw4("The variable full path ",(*irv)->fullpath," doesn't contain the variable name ",
                                      (*irv)->name);

                // A flag to identify if this variable really needs the 2-D coordinate variables.
                short cv_2d_flag = 0;

                // 2-D coordinate variable names for the potential variable candidate
                vector<string> cv_2d_names;

                // Dimension names of the 2-D coordinate variables.
                set<string> cv_2d_dimnames;
                 
                // Loop through the map from dim. name to coordinate name.
                for(map<string,string>::const_iterator itm = cfdimname_to_cvar2dname.begin();
                    itm != cfdimname_to_cvar2dname.end();++itm) {
                    // Obtain the dimension name from the cfdimname.
                    string reduced_dimname = HDF5CFUtil::obtain_string_after_lastslash(itm->first);
//cerr<<"reduced_dimname is "<<reduced_dimname <<endl;
                    string cfdim_path;
                    if(itm->first.size() <= reduced_dimname.size()) 
                        throw2("The cf dim. name of this dimension is not right.",itm->first);
                    else
                        cfdim_path= itm->first.substr(0,itm->first.size() - reduced_dimname.size());
                    // cfdim_path will not be NULL only when the cfdim name is for the 2-D cv var.

//cerr<<"var_path is "<<var_path <<endl;
//cerr<<"cfdim_path is "<<cfdim_path <<endl;
                    // Find the correct path,
                    // Note: 
                    // var_path doesn't have to be the same as cfdim_path
                    // consider the variable /a1/a2/foo and the latitude /a1/latitude(cfdimpath is /a1)
                    // If there is no /a1/a2/latitude, the /a1/latitude can be used as the coordinate of /a1/a2/foo.
                    // 
                    if(var_path == cfdim_path) {
                        for (vector<Dimension*>::iterator ird = (*irv)->dims.begin();
                            ird!=(*irv)->dims.end();++ird) {
                            if(reduced_dimname == (*ird)->name) {
                               cv_2d_flag++;
                               cv_2d_names.push_back(itm->second);
                               cv_2d_dimnames.insert((*ird)->name);
                            }
                        }
                    }
                    
                }

                // Note: 
                // var_path doesn't have to be the same as cfdim_path
                // consider the variable /a1/a2/foo and the latitude /a1/latitude(cfdimpath is /a1)
                // If there is no /a1/a2/latitude, the /a1/latitude can be used as the coordinate of /a1/a2/foo.
                // The variable  has 2 coordinates(dimensions) if the flag is 2
                // But we want to check if var_path is the same as cfdim_path first. So we check cfdimname_to_cvarname again.
                if(cv_2d_flag !=2) {
                    cv_2d_flag = 0;
                    // Loop through the map from dim. name to coordinate name.
                    for(map<string,string>::const_iterator itm = cfdimname_to_cvar2dname.begin();
                        itm != cfdimname_to_cvar2dname.end();++itm) {
                        // Obtain the dimension name from the cfdimname.
                        string reduced_dimname = HDF5CFUtil::obtain_string_after_lastslash(itm->first);
                        string cfdim_path;
                        if(itm->first.size() <= reduced_dimname.size()) 
                            throw2("The cf dim. name of this dimension is not right.",itm->first);
                        else
                            cfdim_path= itm->first.substr(0,itm->first.size() - reduced_dimname.size());
                        // cfdim_path will not be NULL only when the cfdim name is for the 2-D cv var.

                        // Find the correct path,
                        // Note: 
                        // var_path doesn't have to be the same as cfdim_path
                        // consider the variable /a1/a2/foo and the latitude /a1/latitude(cfdimpath is /a1)
                        // If there is no /a1/a2/latitude, the /a1/latitude can be used as the coordinate of /a1/a2/foo.
                        // 
                        if(var_path.find(cfdim_path)!=string::npos) {
                            for (vector<Dimension*>::iterator ird = (*irv)->dims.begin();
                                ird!=(*irv)->dims.end();++ird) {
                                if(reduced_dimname == (*ird)->name) {
                                   cv_2d_flag++;
                                   cv_2d_names.push_back(itm->second);
                                   cv_2d_dimnames.insert((*ird)->name);
                                }
                            }
                        }
                    
                    }
                }
                if(2 == cv_2d_flag) {

                    // Add latitude and longitude to the 'coordinates' attribute.
                    co_attrvalue = cv_2d_names[0] + " " + cv_2d_names[1];
                    if((*irv)->rank >2) {
                        for (vector<Dimension *>::iterator ird = (*irv)->dims.begin();
                            ird !=(*irv)->dims.end();++ird) {

                            // Add 3rd-dimension to the 'coordinates' attribute.
                            if(cv_2d_dimnames.find((*ird)->name) == cv_2d_dimnames.end())
                                co_attrvalue = co_attrvalue + " " +(*ird)->newname;
                        }
                    }
                    Attribute * attr = new Attribute();
                    Add_Str_Attr(attr,co_attrname,co_attrvalue);
                    (*irv)->attrs.push_back(attr);

                }
            }
            
        }

    }
}
// #endif

void GMFile:: Create_Missing_CV(GMCVar *GMcvar, const string& dimname) throw(Exception) {

    GMcvar->name = dimname;
    GMcvar->newname = GMcvar->name;
    GMcvar->fullpath = GMcvar->name;
    GMcvar->rank = 1;
    GMcvar->dtype = H5INT32;
    hsize_t gmcvar_dimsize = dimname_to_dimsize[dimname];
    Dimension* gmcvar_dim = new Dimension(gmcvar_dimsize);
    gmcvar_dim->name = dimname;
    gmcvar_dim->newname = dimname;
    GMcvar->dims.push_back(gmcvar_dim);
    GMcvar->cfdimname = dimname;
    GMcvar->cvartype = CV_NONLATLON_MISS;
    GMcvar->product_type = product_type;
}

 // Check if this is just a netCDF-4 dimension. We need to check the dimension scale dataset attribute "NAME",
 // the value should start with "This is a netCDF dimension but not a netCDF variable".
bool GMFile::Is_netCDF_Dimension(Var *var) throw(Exception) {
    
    string netcdf_dim_mark = "This is a netCDF dimension but not a netCDF variable";

    bool is_only_dimension = false;

    for(vector<Attribute *>::iterator ira = var->attrs.begin();
          ira != var->attrs.end();ira++) {

        if ("NAME" == (*ira)->name) {

             Retrieve_H5_Attr_Value(*ira,var->fullpath);
             string name_value;
             name_value.resize((*ira)->value.size());
             copy((*ira)->value.begin(),(*ira)->value.end(),name_value.begin());

             // Compare the attribute "NAME" value with the string netcdf_dim_mark. We only compare the string with the size of netcdf_dim_mark
             if (0 == name_value.compare(0,netcdf_dim_mark.size(),netcdf_dim_mark))
                is_only_dimension = true;
           
            break;
        }
    } // for(vector<Attribute *>::iterator ira = var->attrs.begin(); ...

    return is_only_dimension;
}

void 
GMFile::Handle_SpVar_Attr() throw(Exception) {

}
    







