#ifndef EAMXX_MAM_OPTICS_HPP
#define EAMXX_MAM_OPTICS_HPP

#include <physics/mam/mam_coupling.hpp>
#include <share/atm_process/atmosphere_process.hpp>
#include <share/util/scream_common_physics_functions.hpp>
#include <share/atm_process/ATMBufferManager.hpp>

#include <ekat/ekat_parameter_list.hpp>
#include <ekat/ekat_workspace.hpp>
#include <mam4xx/mam4.hpp>

#include <string>

#ifndef KOKKOS_ENABLE_CUDA
#define protected_except_cuda public
#define private_except_cuda public
#else
#define protected_except_cuda protected
#define private_except_cuda private
#endif

namespace scream
{

// The process responsible for handling MAM4 aerosol optical properties. The AD
// stores exactly ONE instance of this class in its list of subcomponents.
class MAMOptics final : public scream::AtmosphereProcess {
  using PF = scream::PhysicsFunctions<DefaultDevice>;
  using KT = ekat::KokkosTypes<DefaultDevice>;

  // a quantity stored in a single vertical column with a single index
  using ColumnView = mam4::ColumnView;

  // a thread team dispatched to a single vertical column
  using ThreadTeam = mam4::ThreadTeam;

public:

  // Constructor
  MAMOptics(const ekat::Comm& comm, const ekat::ParameterList& params);

protected_except_cuda:

  // --------------------------------------------------------------------------
  // AtmosphereProcess overrides (see share/atm_process/atmosphere_process.hpp)
  // --------------------------------------------------------------------------

  // process metadata
  AtmosphereProcessType type() const override;
  std::string name() const override;

  // grid
  void set_grids(const std::shared_ptr<const GridsManager> grids_manager) override;

  // process behavior
  void initialize_impl(const RunType run_type) override;
  void run_impl(const double dt) override;
  void finalize_impl() override;

private_except_cuda:
  // state variable
  mam_coupling::view_2d state_q_, ext_cmip6_lw_, qqcw_;// odap_aer_,

  mam_coupling::complex_view_2d specrefndxlw_;

  // number of horizontal columns and vertical levels
  int ncol_, nlev_;

  // number of shortwave and longwave radiation bands
  int nswbands_, nlwbands_;
  // FIXME: we need to save this values in a different file.
  Kokkos::complex<Real> crefwlw_[mam4::modal_aer_opt::nlwbands];
  Kokkos::complex<Real> crefwsw_[mam4::modal_aer_opt::nswbands];

  mam_coupling::view_3d absplw_[mam4::AeroConfig::num_modes()][mam4::modal_aer_opt::nlwbands];
  mam_coupling::view_1d refrtablw_[mam4::AeroConfig::num_modes()][mam4::modal_aer_opt::nlwbands];
  mam_coupling::view_1d refitablw_[mam4::AeroConfig::num_modes()][mam4::modal_aer_opt::nlwbands];

  mam_coupling::view_2d mass_, radsurf_, logradsurf_  ;
  mam_coupling::view_3d cheb_, dgnumwet_m_, dgnumdry_m_;
  mam_coupling::complex_view_3d specrefindex_;
  mam_coupling::view_3d qaerwat_m_, ext_cmip6_lw_inv_m_;
  // FIXME: move this values to mam_coupling
  mam_coupling::const_view_2d z_mid_, z_iface_, p_int_, p_del_;

  // MAM4 aerosol particle size description
  mam4::AeroConfig aero_config_;

  // atmospheric and aerosol state variables
  // mam_coupling::WetAtmosphere wet_atm_;
  mam_coupling::DryAtmosphere dry_atm_;
  mam_coupling::AerosolState  wet_aero_;//,

  // aerosol processes
  //std::unique_ptr<mam4::OpticsProcess> optics_;
  // std::unique_ptr<mam4::CalcSizeProcess> calcsize_process_;

  // physics grid for column information
  std::shared_ptr<const AbstractGrid> grid_;
}; // MAMOptics

} // namespace scream

#endif // EAMXX_MAM_OPTICS_HPP
