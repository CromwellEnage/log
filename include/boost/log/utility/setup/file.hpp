/*
 *          Copyright Andrey Semashev 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   file.hpp
 * \author Andrey Semashev
 * \date   16.05.2008
 *
 * The header contains implementation of convenience functions for enabling logging to a file.
 */

#ifndef BOOST_LOG_UTILITY_SETUP_FILE_HPP_INCLUDED_
#define BOOST_LOG_UTILITY_SETUP_FILE_HPP_INCLUDED_

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/parameter/preprocessor.hpp>
#include <boost/log/detail/config.hpp>
#include <boost/log/detail/sink_init_helpers.hpp>
#include <boost/log/detail/parameter_tools.hpp>
#include <boost/log/core/core.hpp>
#ifndef BOOST_LOG_NO_THREADS
#include <boost/log/sinks/sync_frontend.hpp>
#else
#include <boost/log/sinks/unlocked_frontend.hpp>
#endif
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/keywords/scan_method.hpp>
#include <boost/log/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifndef BOOST_LOG_DOXYGEN_PASS
#ifndef BOOST_LOG_NO_THREADS
#define BOOST_LOG_FILE_SINK_FRONTEND_INTERNAL sinks::synchronous_sink
#else
#define BOOST_LOG_FILE_SINK_FRONTEND_INTERNAL sinks::unlocked_sink
#endif
#endif // BOOST_LOG_DOXYGEN_PASS

namespace boost {

BOOST_LOG_OPEN_NAMESPACE

namespace aux {

//! The function creates a file collector according to the specified arguments
template< typename ArgsT >
inline shared_ptr< sinks::file::collector > setup_file_collector(ArgsT const&, mpl::true_ const&)
{
    return shared_ptr< sinks::file::collector >();
}
template< typename ArgsT >
inline shared_ptr< sinks::file::collector > setup_file_collector(ArgsT const& args, mpl::false_ const&)
{
    return sinks::file::make_collector(args);
}

} // namespace aux

#ifndef BOOST_LOG_DOXYGEN_PASS

BOOST_PARAMETER_BASIC_FUNCTION(
    (shared_ptr< BOOST_LOG_FILE_SINK_FRONTEND_INTERNAL< sinks::text_file_backend > >),
    add_file_log,
    keywords,
    (
        (required (file_name, *))
        (optional (open_mode, *))
        (optional (rotation_size, *))
        (optional (time_based_rotation, *))
        (optional (auto_flush, *))
        (optional (target, *))
        (optional (max_size, *))
        (optional (min_free_space, *))
        (optional (max_files, *))
        (optional (scan_method, *))
        (optional (filter, *))
        (optional (format, *))
    )
)
{
    typedef sinks::text_file_backend backend_t;
    shared_ptr< backend_t > pBackend = boost::make_shared< backend_t >(args);

    shared_ptr< sinks::file::collector > pCollector = aux::setup_file_collector(args,
        typename is_void< typename parameter::binding< Args, keywords::tag::target, void >::type >::type());
    if (pCollector)
    {
        pBackend->set_file_collector(pCollector);
        pBackend->scan_for_files(args[keywords::scan_method | sinks::file::scan_matching]);
    }

    shared_ptr< BOOST_LOG_FILE_SINK_FRONTEND_INTERNAL< backend_t > > pSink =
        boost::make_shared< BOOST_LOG_FILE_SINK_FRONTEND_INTERNAL< backend_t > >(pBackend);

    aux::setup_filter(*pSink, args,
        typename is_void< typename parameter::binding< Args, keywords::tag::filter, void >::type >::type());

    aux::setup_formatter(*pSink, args,
        typename is_void< typename parameter::binding< Args, keywords::tag::format, void >::type >::type());

    core::get()->add_sink(pSink);

    return pSink;
}

#else // BOOST_LOG_DOXYGEN_PASS

/*!
 * The function initializes the logging library to write logs to a file stream.
 *
 * \param args A number of named arguments. The following parameters are supported:
 *             \li \c file_name The file name or its pattern. This parameter is mandatory.
 *             \li \c open_mode The mask that describes the open mode for the file. See <tt>std::ios_base::openmode</tt>.
 *             \li \c rotation_size The size of the file at which rotation should occur. See <tt>basic_text_file_backend</tt>.
 *             \li \c time_based_rotation The predicate for time-based file rotations. See <tt>basic_text_file_backend</tt>.
 *             \li \c auto_flush A boolean flag that shows whether the sink should automatically flush the file
 *                               after each written record.
 *             \li \c target The target directory to store rotated files in. See <tt>sinks::file::make_collector</tt>.
 *             \li \c max_size The maximum total size of rotated files in the target directory. See <tt>sinks::file::make_collector</tt>.
 *             \li \c min_free_space Minimum free space in the target directory. See <tt>sinks::file::make_collector</tt>.
 *             \li \c max_files The maximum total number of rotated files in the target directory. See <tt>sinks::file::make_collector</tt>.
 *             \li \c scan_method The method of scanning the target directory for log files. See <tt>sinks::file::scan_method</tt>.
 *             \li \c filter Specifies a filter to install into the sink. May be a string that represents a filter,
 *                           or a filter lambda expression.
 *             \li \c format Specifies a formatter to install into the sink. May be a string that represents a formatter,
 *                           or a formatter lambda expression (either streaming or Boost.Format-like notation).
 * \return Pointer to the constructed sink.
 */
template< typename... ArgsT >
shared_ptr< BOOST_LOG_FILE_SINK_FRONTEND_INTERNAL< sinks::text_file_backend > > add_file_log(ArgsT... const& args);

#endif // BOOST_LOG_DOXYGEN_PASS

BOOST_LOG_CLOSE_NAMESPACE // namespace log

} // namespace boost

#undef BOOST_LOG_FILE_SINK_FRONTEND_INTERNAL

#include <boost/log/detail/footer.hpp>

#endif // BOOST_LOG_UTILITY_SETUP_FILE_HPP_INCLUDED_
