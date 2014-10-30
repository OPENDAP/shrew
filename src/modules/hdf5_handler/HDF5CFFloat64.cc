// This file is part of hdf5_handler: an HDF5 file handler for the OPeNDAP
// data server.

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

////////////////////////////////////////////////////////////////////////////////
/// \file HDF5CFFloat64.cc
/// \brief The implementation of mapping HDF5 float to DAP float for the CF option 
///
/// In the future, this may be merged with the default option.
/// \author Muqun Yang <myang6@hdfgroup.org>
///
////////////////////////////////////////////////////////////////////////////////

#include "config_hdf5.h"

#include "InternalErr.h"
#include "HDF5CFFloat64.h"

HDF5CFFloat64::HDF5CFFloat64(const string &n, const string &d):Float64(n, d)
{
}

HDF5CFFloat64::~HDF5CFFloat64()
{
}
BaseType *HDF5CFFloat64::ptr_duplicate()
{
    return new HDF5CFFloat64(*this);
}

bool HDF5CFFloat64::read()
{
    throw InternalErr(__FILE__, __LINE__,
                      "Unimplemented read method called.");
}
