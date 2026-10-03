//
// Copyright 2019 Ettus Research, a National Instruments Brand
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include <uhd/config.hpp>
#include <uhd/rfnoc/noc_block_base.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/types/time_spec.hpp>
#include <boost/optional.hpp>
#include <optional>

namespace uhd { namespace rfnoc {

/*! DUC Block Control Class
 *
 * \ingroup rfnoc_blocks
 *
 * # Overview
 *
 * The DUC is a multi-channel digital upconverter with a built-in DDS
 * frequency shifter. It is commonly placed directly before a radio block to
 * increase the sampling rate and shift a signal to the desired transmit
 * frequency.
 *
 * The block processes signed complex 16-bit samples (`sc16`). Each channel has
 * an independent frequency shift and interpolation setting. The number of
 * channels and the maximum supported interpolation are configured in the FPGA
 * at synthesis time.
 * The block controller reads these maximum capabilities, stored in capability
 * registers, during initialization.
 *
 * # Features
 *
 * - Per-channel frequency shifting with optional timed commands.
 * - Integer interpolation using a cascade of half-band filters followed by a
 *   CIC interpolator.
 * - IQ scaling to compensate for the gain of the interpolation filters.
 * - Independent rate and frequency configuration for every channel.
 *
 * # Theory of Operation
 *
 * <img src="duc_signal_path.svg" width="1000" >
 *
 *
 * The total interpolation is the product of the CIC interpolation and the
 * half-band interpolation stages. With `NUM_HB` half-band stages and a CIC
 * limit of `CIC_MAX_INTERP`, the maximum supported interpolation is
 * \f$\mathrm{CIC\_MAX\_INTERP} \times 2^{\mathrm{NUM\_HB}}\f$.
 *
 * # FPGA Compile-Time Configuration
 *
 * The DUC FPGA block is configured through the following parameters:
 *
 * - `NUM_PORTS`: Number of independent DUC channels.
 * - `NUM_HB`: Number of half-band filter stages in each channel.
 * - `CIC_MAX_INTERP`: Maximum interpolation through CIC filter stage in each channel.
 * - `NIPC`: Number of samples processed per clock cycle. `NIPC == 1` selects
 *   the legacy single-sample implementation. Values greater than one select
 *   the multisample implementation, which processes samples in parallel for
 *   wideband images.
 *
 * The in-tree DUC YAML descriptor derives `NIPC` from the configured RF
 * bandwidth. Each parallel processing chain is sized for approximately
 * 200 MHz of RF bandwidth, with the resulting value rounded up to a power of
 * two. Customized FPGA images may choose a different value based on their
 * clock rates and resource budget.
 *
 * # Runtime Configuration
 *
 * This block exposes two user-configurable configuration parameters per
 * channel:
 *
 * - `freq`: Frequency shift in Hz. The `set_freq()` convenience method should
 *   be preferred over setting the property directly because it also handles a
 *   command time.
 * - `interp`: Integer interpolation value.
 *
 * The interpolation value can be changed at runtime, but a change is applied
 * only between bursts. Once an interpolation value has been applied, it remains
 * in effect for the entire burst.
 *
 * The legacy single-sample DUC and the multisample (wideband) DUC react
 * differently to timed and untimed frequency-shift commands:
 *
 * - **Legacy single-sample implementation:**
 *   - **Untimed commands:** Configure the frequency shift for the next IQ
 *     sample processed by the DUC and all subsequent samples until the next
 *     frequency change. If the DUC has not processed data since the command
 *     was issued, a subsequent untimed command overwrites the earlier one.
 *   - **Timed commands:** Follow the standard timed-command mechanism. The
 *     command is held until the IQ sample corresponding to its timestamp is
 *     processed, and the new frequency shift applies to that sample and all
 *     subsequent samples until the next frequency change.
 *
 * - **Multisample (wideband) implementation:** Timed and untimed commands are
 *   stored in the same 32-entry FPGA command queue. Commands are applied only
 *   when the DUC is actively processing data, and only the command at the
 *   front of the queue can be applied.
 *   - **Untimed commands:** An untimed command waits for all commands already
 *     ahead of it in the queue, including timed commands. It is then applied
 *     on the next data transfer. Consequently, successive untimed commands
 *     are not collapsed or skipped; each remains active for at least one data
 *     transfer containing `NIPC` IQ samples.
 *   - **Timed commands:** A timed command waits in the queue until it reaches
 *     the front and its requested timestamp has been reached. It is applied
 *     starting with the data transfer containing the IQ sample corresponding
 *     to that timestamp and remains active until the next frequency change.
 *
 * In the multisample implementation, timed frequency shifts have word-level
 * rather than sample-level granularity. One word is one data transfer containing
 * `NIPC` samples (see [NIPC](@ref axi_stream_data_simple_interface_anchor)).
 * Therefore, if the requested timestamp falls within a word, the new frequency
 * shift will be applied to samples earlier in that same word. For example, if
 * the timestamp corresponds to the third sample in a word of eight samples,
 * the new frequency shift will be applied to all eight samples in that word,
 * including the first two samples preceding the requested timestamp, and to
 * all subsequent samples until the next frequency change. The exact sample-level
 * transition is not guaranteed.
 *
 * # Register Maps and Compatibility
 *
 * The DUC has two register-map generations:
 *
 * - Compatibility major 0 is the legacy single-sample register map, used by
 *   implementations for bandwidths up to 200 MHz.
 * - Compatibility major 1 is the multisample register map, used by the
 *   wideband implementation.
 *
 * The block controller reads the FPGA compatibility number from
 * `REG_COMPAT_NUM` and selects the corresponding register map. An unsupported
 * major version is rejected. `REG_ADDRS_V0` and `REG_ADDRS_V1` contain the
 * version-specific addresses used by the controller.
 *
 */
class UHD_API duc_block_control : public noc_block_base
{
public:
    RFNOC_DECLARE_BLOCK(duc_block_control)

    //! Version-specific register addresses
    struct reg_addrs_t
    {
        uint16_t major_compat;
        uint16_t minor_compat;
        uint32_t num_hb;
        uint32_t cic_max_interp;
        std::optional<uint32_t> n_addr;
        std::optional<uint32_t> m_addr;
        std::optional<uint32_t> config_addr;
        uint32_t interp_addr;
        uint32_t freq_addr;
        uint32_t scale_iq_addr;
        uint32_t time_incr_addr;
    };

    // Compat register address (same across all versions)
    static const uint32_t REG_COMPAT_NUM;
    // Register addresses for version 0.x
    static const reg_addrs_t REG_ADDRS_V0;
    // Register addresses for version 1.x
    static const reg_addrs_t REG_ADDRS_V1;

    /*! Set the DDS frequency
     *
     * This block applies the frequency shift after interpolation. The frequency
     * is specified in Hz rather than as a normalized frequency; the valid range
     * is from -get_output_rate()/2 to +get_output_rate()/2.
     *
     * Note: When the sample rate is modified, the frequency shift is kept
     * constant.
     * Because the FPGA internally uses a relative phase increment, changing
     * the input sampling rate will trigger a property propagation to
     * recalculate the phase increment based off of this value.
     *
     * For the multisample implementation, timed and untimed frequency
     * commands are queued in a 32-entry FPGA queue. While the DUC is actively
     * processing data, queued commands are processed one at a time; an untimed
     * command is applied on the next transfer, while a timed command also waits
     * for its requested timestamp.
     *
     * This function will coerce the frequency to a valid value, and return the
     * coerced value.
     *
     * \param freq The frequency shift in Hz
     * \param chan The channel to which this change shall be applied
     * \param time When to apply the new frequency
     * \returns The coerced, actual current frequency of the DDS
     */
    virtual double set_freq(const double freq,
        const size_t chan,
        const std::optional<uhd::time_spec_t> time = {}) = 0;

    double set_freq(const double freq, const size_t chan, const uhd::time_spec_t time)
    {
        return set_freq(freq, chan, std::make_optional(time));
    }

    [[deprecated("Prefer std::optional over boost::optional.")]] virtual double set_freq(
        const double freq,
        const size_t chan,
        const boost::optional<uhd::time_spec_t> time)
    {
        return set_freq(
            freq, chan, bool(time) ? std::make_optional(*time) : std::nullopt);
    }

    /*! Return the current DDS frequency
     *
     * \returns The current frequency of the DDS
     */
    virtual double get_freq(const size_t chan) const = 0;

    /*! Return the range of frequencies that \p chan can be set to.
     *
     * The frequency shifter is the last component in the DUC, and thus can
     * shift frequencies (digitally) between -get_output_rate()/2
     * and +get_output_rate()/2.
     *
     * The returned values are in Hz (not normalized frequencies) and are valid
     * inputs for set_freq().
     *
     * \return The range of frequencies that the DUC can shift the input by
     */
    virtual uhd::freq_range_t get_frequency_range(const size_t chan) const = 0;

    /*! Return the sampling rate at this block's input
     *
     * \param chan The channel for which the rate is being queried
     * \returns the sampling rate at this block's input
     */
    virtual double get_input_rate(const size_t chan) const = 0;

    /*! Return the sampling rate at this block's output
     *
     * This is equivalent to calling get_input_rate() multiplied by the interpolation
     *
     * \param chan The channel for which the rate is being queried
     * \returns the sampling rate at this block's input
     */
    virtual double get_output_rate(const size_t chan) const = 0;

    /*! Manually set the sampling rate at this block's output
     *
     * \param rate The requested rate
     * \param chan The channel for which the rate is being set
     */
    virtual void set_output_rate(const double rate, const size_t chan) = 0;

    /*! Return a range of valid input rates, based on the current output rate
     *
     * Note the return value is only valid as long as the output rate does not
     * change.
     */
    virtual uhd::meta_range_t get_input_rates(const size_t chan) const = 0;

    /*! Attempt to set the input rate of this block
     *
     * This will set the interpolation such that the output rate is untouched, and
     * that the output rate divided by the new interpolation is as close as
     * possible to the requested \p rate.
     *
     * \param rate The requested rate
     * \param chan The channel for which the rate is being queried
     * \returns the coerced sampling rate at this block's output
     */
    virtual double set_input_rate(const double rate, const size_t chan) = 0;
};

}} // namespace uhd::rfnoc
